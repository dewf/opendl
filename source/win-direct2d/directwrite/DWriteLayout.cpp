#include "DWriteLayout.h"

#include "../unicodestuff.h"
#include "../private_defs.h"
#include "../classes/CT/CTFont.h"
#include "CoreTextFormatSpec.h"
#include "../COMStuff.h"

#include <unicode/unistr.h>
#include <unicode/ubidi.h>

DWriteLayout::DWriteLayout(cf::AttributedStringRef attrString)
	:attrString(attrString->copy())
{
	// the real font(s) should be in the attributed string somewhere ...
	static auto defaultFont = CTFont::getDefaultFont();

	auto utf8 = attrString->getString()->getStdString();

	// create default formatter
	auto attrWideString = utf8_to_wstring(utf8);
	HR(writeFactory->CreateTextLayout(
		attrWideString.c_str(),
		(UINT32)attrWideString.length(),
		defaultFont->getTextFormat(),
		FLT_MAX, // no width yet
		FLT_MAX, // no height yet
		&layout));

	// set RTL direction if necessary
	auto text = icu::UnicodeString::fromUTF8(utf8.c_str());
	auto dir = ubidi_getBaseDirection(text.getBuffer(), text.length());
	if (dir == UBIDI_RTL) {
		layout->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
		rightToLeft = true;
	}

	// apply the attributes to the layout so it's ready for accurate metrics
	auto ranges = attrString->getRanges();
	for (auto i = ranges.begin(); i != ranges.end(); i++) {
		auto dwRange = dwTextRangeFromDLRange(i->range);
		// directwrite effects (anything not covered by built-in stuff, and that requires a valid target [which we don't have here] to set)
		// set all the built-ins (font family/size/weight/underline/etc)
		auto effectSpec = new CoreTextFormatSpec(i->attrs);
		effectSpec->applyToLayout(layout, dwRange);
		effectSpec->Release(); // (COM, not cf::Object): should be retained by layout, correct?
	}
}

DWriteLayout::~DWriteLayout()
{
	assert(lines == nullptr);
	assert(currentRuns == nullptr);
	//
	if (lineMetrics) delete[] lineMetrics;
	::SafeRelease(&layout);
	attrString->release();
}

void DWriteLayout::currentRunsToLine()
{
	auto line = new CTLine(currentRuns, currentLineWidth, &lineMetrics[mLineIndex], this, linesOwnLayout);
	lines->appendValue(line);
	line->release();
	//
	currentRuns->removeAllValues();
	//
	mLineIndex++;
	mCharIndex = 0;
	currentLineWidth = 0;
}

void DWriteLayout::prescan()
{
	// capture all the lines and glyph runs etc
	layout->GetLineMetrics(NULL, 0, &numLines);
	if (numLines > 0) {
		// get all the various metrics needed later (by CTLine etc)
		layout->GetMetrics(&textMetrics);
		layout->GetLineSpacing(&lineSpacingMethod, &lineSpacing, &baseline);

		lineMetrics = new DWRITE_LINE_METRICS[numLines];
		layout->GetLineMetrics(lineMetrics, numLines, &numLines); // can't use NULL the 2nd time, silly

		//lines = new cf::MutableArray(); // keep adding currentLine to this
		layout->Draw(nullptr, this, 0, 0); // use self as a callback object, to capture the line content

		// finish any remaining lines
		if (currentRuns->getCount() > 0) {
			currentRunsToLine();
		}

		// set the actual number of lines post-callbacks, since the content might be truncated depending on DWRITE_TRIMMING mode
		numLines = mLineIndex;
	}
}

CTLineRef DWriteLayout::layoutSingleLine()
{
	// temporary storage for prescan
	currentRuns = new cf::MutableArray();
	lines = new cf::MutableArray();

	linesOwnLayout = true; // CTLine ctor will AddRef() this layout
	prescan();
	auto line = ((CTLineRef)lines->getValueAtIndex(0))->retain(); // retain for caller ownership / protection from array releases below

	// free temp storage
	currentRuns->release();
	currentRuns = nullptr;
	lines->release();
	lines = nullptr;

	return line;
}

cf::ArrayRef DWriteLayout::layoutArea(dl_CGFloat width, dl_CGFloat height)
{
	// temporary storage for prescan
	currentRuns = new cf::MutableArray();
	lines = new cf::MutableArray();

	layout->SetMaxWidth((FLOAT)width);
	layout->SetMaxHeight((FLOAT)height);

	// trim text beyond height
	DWRITE_TRIMMING trimOpts = {};
	trimOpts.granularity = DWRITE_TRIMMING_GRANULARITY_WORD;
	layout->SetTrimming(&trimOpts, NULL);

	prescan();

	auto result = lines->retain(); // retain for caller ownership / protection from release below

	// free temp storage
	currentRuns->release();
	currentRuns = nullptr;
	lines->release();
	lines = nullptr;

	return result;
}

IFACEMETHODIMP DWriteLayout::DrawGlyphRun(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode,
	__in DWRITE_GLYPH_RUN const* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
	IUnknown* clientDrawingEffect)
{
	auto currentLineMetrics = lineMetrics[mLineIndex];
	bool inTrailingWhitespace = ((UINT32)mCharIndex >= (currentLineMetrics.length - currentLineMetrics.trailingWhitespaceLength));

	auto run = new CTRun(
		baselineOriginX,
		baselineOriginY,
		measuringMode,
		glyphRun,
		glyphRunDescription,
		clientDrawingEffect);
	currentRuns->appendValue(run);
	run->release();

	// get actual line width, don't know where else to get this
	for (auto i = 0; i < glyphRun->glyphCount; i++) {
		currentLineWidth += glyphRun->glyphAdvances[i];
	}

	mCharIndex += glyphRunDescription->stringLength;
	if (mCharIndex == currentLineMetrics.length) // end of line
	{
		currentRunsToLine();
	}

	return S_OK;
}


//void StackLine::ctorCommon()
//{
//	// copy the memory pointed to by the runs so it stays alive even after the layout->Draw (cumulative IDWriteTextRenderer callbacks)
//	// this is a somewhat risky optimization to avoid a gazillion memory allocations for each line/frame (5 per individual run)
//	//   - won't necessarily always work if Microsoft changes their internal code
//	int glyphCountSum = 0;
//	int strLenSum = 0;
//	for (auto i = runs.begin(); i != runs.end(); i++) {
//		glyphCountSum += i->glyphRun.glyphCount;
//		strLenSum += i->glyphRunDescription.stringLength;
//	}
//	auto first = runs.front();
//
//	indices = std::vector<UINT16>(&first.glyphRun.glyphIndices[0], &first.glyphRun.glyphIndices[glyphCountSum]);
//
//	// apparently some of these are optional!
//	bool hasAdvances = false;
//	if (first.glyphRun.glyphAdvances) {
//		advances = std::vector<FLOAT>(&first.glyphRun.glyphAdvances[0], &first.glyphRun.glyphAdvances[glyphCountSum]);
//		hasAdvances = true;
//	}
//	bool hasOffsets = false;
//	if (first.glyphRun.glyphOffsets) {
//		offsets = std::vector<DWRITE_GLYPH_OFFSET>(&first.glyphRun.glyphOffsets[0], &first.glyphRun.glyphOffsets[glyphCountSum]);
//		hasOffsets = true;
//	}
//	//
//	strings = std::vector<WCHAR>(&first.glyphRunDescription.string[0], &first.glyphRunDescription.string[strLenSum]);
//
//	bool hasClusterMaps = false;
//	if (first.glyphRunDescription.clusterMap) {
//		clusterMaps = std::vector<UINT16>(&first.glyphRunDescription.clusterMap[0], &first.glyphRunDescription.clusterMap[strLenSum]);
//		hasClusterMaps = true;
//	}
//
//	// fixup internal run addresses
//	int glyphCountPos = 0; // up to sum
//	int strLenPos = 0; // same
//	for (auto i = runs.begin(); i != runs.end(); i++) {
//		i->glyphRun.glyphIndices = (UINT16 *)(indices.data() + glyphCountPos);
//		i->glyphRun.glyphAdvances = hasAdvances ? ((FLOAT *)(advances.data() + glyphCountPos)) : nullptr;
//		i->glyphRun.glyphOffsets = hasOffsets ? ((DWRITE_GLYPH_OFFSET *)(offsets.data() + glyphCountPos)) : nullptr;
//		//
//		i->glyphRunDescription.string = (WCHAR *)(strings.data() + strLenPos);
//		i->glyphRunDescription.clusterMap = hasClusterMaps ? ((UINT16 *)(clusterMaps.data() + strLenPos)) : nullptr;
//		//
//		glyphCountPos += i->glyphRun.glyphCount;
//		strLenPos += i->glyphRunDescription.stringLength;
//	}
//}
//
//StackLine::StackLine(std::vector<StackRun> in_runs, DWRITE_LINE_METRICS * metrics)
//	:runs(std::move(in_runs)),
//	lineMetrics(*metrics)
//{
//	ctorCommon();
//}
//
//StackLine::StackLine(const StackLine &b)
//	:runs(b.runs),
//	lineMetrics(b.lineMetrics)
//{
//	ctorCommon();
//}
