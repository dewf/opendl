#include "CTLine.h"

#include "../CT/CTRun.h"
#include <assert.h>
#include "../../unicodestuff.h"

#include "../../directwrite/DWriteLayout.h"

CTLine::CTLine(cf::ArrayRef runs, dl_CGFloat lineWidth, DWRITE_LINE_METRICS *metrics, DWriteLayout *layout, bool layoutOwned)
	:runs(runs->copy()),
	 lineWidth(lineWidth),
	 lineMetrics(*metrics),
	 layout(layout),
	 layoutOwned(layoutOwned)
{
	if (layoutOwned) {
		layout->AddRef();
	}
	auto first = (CTRunRef)runs->_getVector().front();
	auto last = (CTRunRef)runs->_getVector().back();
	auto start = first->getStringRange().location;
	auto end = last->getStringRange().location + last->getStringRange().length;
	textRange = dl_CFRangeMake(start, end - start);
}

CTLine::~CTLine()
{
	if (layout && layoutOwned) {
		::SafeRelease(&layout);
	}
	runs->release();
}

// static method
CTLineRef CTLine::createFromAttrString(cf::AttributedStringRef attrString)
{
	auto layout = new DWriteLayout(attrString);
	auto line = layout->layoutSingleLine();
	layout->Release(); // because the line owns the layout now
	return line;
}

//
double CTLine::getTypographicBounds(dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *leading)
{
	DWRITE_FONT_METRICS fm;
	dl_CGFloat maxDescent = -99999999;
	dl_CGFloat maxLeading = -99999999;

	for (auto &i : runs->_getVector()) {
		auto run = (CTRunRef)i;
		run->glyphRun.fontFace->GetMetrics(&fm);
		maxDescent = max(maxDescent, (fm.descent * run->glyphRun.fontEmSize) / fm.designUnitsPerEm);
		maxLeading = max(maxLeading, (fm.lineGap * run->glyphRun.fontEmSize) / fm.designUnitsPerEm);
	}
	if (ascent) {
		*ascent = lineMetrics.baseline;
	}
	if (descent) {
		*descent = maxDescent;
	}
	if (leading) {
		*leading = maxLeading;
	}
	return layout->textMetrics.width;
}

//dl_kCTLineBoundsExcludeTypographicLeading = 1 << 0,
//dl_kCTLineBoundsExcludeTypographicShifts = 1 << 1,
//dl_kCTLineBoundsUseHangingPunctuation = 1 << 2,
//dl_kCTLineBoundsUseGlyphPathBounds = 1 << 3,
//dl_kCTLineBoundsUseOpticalBounds = 1 << 4
dl_CGRect CTLine::getBoundsWithOptions(dl_CTLineBoundsOptions options)
{
	dl_CGFloat leading;
	getTypographicBounds(nullptr, nullptr, &leading);

	if (options == 0 || options == dl_kCTLineBoundsUseOpticalBounds) { // these two seem the same on OSX
		//auto origin = getOrigin();
		dl_CGFloat x = 0;
		//if (layout->rightToLeft) {
		//	x = origin.x - lineWidth;
		//}
		return dl_CGRectMake(x, -lineMetrics.baseline, lineWidth, lineMetrics.height + leading);
	}
	else if (options == dl_kCTLineBoundsUseGlyphPathBounds) {
		std::vector<ID2D1Geometry *> runGeoms;
		for (auto &i : runs->_getVector()) {
			auto run = (CTRunRef)i;
			auto geom = getGlyphRunGeometry(
				run->baselineOriginX,
				run->baselineOriginY,
				&run->glyphRun
			);
			runGeoms.push_back(geom);
		}
		// group it
		if (runGeoms.size() > 0) {
			ID2D1GeometryGroup *grouped;
			HR(d2dFactory->CreateGeometryGroup(
				D2D1_FILL_MODE_WINDING, // doesn't matter, non-overlapping
				runGeoms.data(),
				(UINT32)runGeoms.size(),
				&grouped
			));
			for (auto &i : runGeoms) {
				i->Release();
			}
			D2D1_RECT_F bounds;
			grouped->GetBounds(NULL, &bounds);
			grouped->Release();
			auto firstRun = (CTRunRef)runs->getValueAtIndex(0);
			auto y = -(firstRun->baselineOriginY - bounds.top);
			return dl_CGRectMake(bounds.left, y, bounds.right - bounds.left, bounds.bottom - bounds.top);
		}
		else {
			return dl_CGRectMake(0, 0, layout->textMetrics.width, lineMetrics.height);
		}
	}
	else {
		return dl_CGRectMake(0, 0, layout->textMetrics.width, lineMetrics.height);
	}
}

void CTLine::draw(CGContextRef context)
{
	auto textPos = context->getTextPosition();
	auto pos = dl_CGPointMake(textPos.x, textPos.y - lineMetrics.baseline);
	// "delegate" to our layout, which actually knows how to draw
	layout->drawAtPosition(context, pos);
}

cf::ArrayRef CTLine::getGlyphRuns() {
	return runs; // get rule, caller must retain if they want it (and that's a bad idea with all this, since deep down the data all belongs to the DirectWrite layout)
}

dl_CGFloat CTLine::getOffsetForStringIndex(dl_CFIndex charIndex, dl_CGFloat *secondaryOffset) {
	auto idwtl = layout->getRawLayout();
	FLOAT pointX, pointY;
	DWRITE_HIT_TEST_METRICS metrics;
	HR(idwtl->HitTestTextPosition((UINT32)charIndex, FALSE, &pointX, &pointY, &metrics));
	// must be relative to origin
	auto origin = getOrigin();
	pointX -= (FLOAT)origin.x;
	if (secondaryOffset) {
		*secondaryOffset = pointX; // not sure ...
	}
	return pointX;
}

dl_CFIndex CTLine::getStringIndexForPosition(dl_CGPoint position)
{
	auto idwtl = layout->getRawLayout();
	BOOL isTrailingHit, isInside;
	DWRITE_HIT_TEST_METRICS metrics;
	// incoming position is relative to line origin
	// but we need it 'absolute' (relative to layout) for DirectWrite methods
	auto origin = getOrigin();
	position.x += origin.x;
	position.y += origin.y;
	HR(idwtl->HitTestPoint((FLOAT)position.x, (FLOAT)position.y, &isTrailingHit, &isInside, &metrics));
	if (isInside) {
		if (metrics.textPosition >= textRange.location &&
			(metrics.textPosition < dl_CFRangeEnd(textRange)))
		{
			//printf("hit testing: position %d - length %d\n", metrics.textPosition, metrics.length);
			auto extra = isTrailingHit ? 1 : 0;
			return metrics.textPosition + extra;
		}
	}
	return dl_kCFNotFound;
}

dl_CFRange CTLine::getStringRange()
{
	return textRange;
}

dl_CGPoint CTLine::getOrigin() {
	auto first = (CTRunRef) runs->getValueAtIndex(0);
	auto p = first->getXY();
	if (layout->rightToLeft) {
		p.x -= lineWidth;
		return p;
	}
	else {
		return p;
	}
}
