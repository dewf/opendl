#pragma once

#include "../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../opendl.h"
#include "CoreTextRenderer.h"
#include "MyTextRenderer.h"

#include "../classes/CT/CTLine.h"
#include "../classes/CT/CTRun.h"

class DWriteLayout : public MyTextRenderer {
	cf::AttributedStringRef attrString = nullptr;
	IDWriteTextLayout *layout = nullptr;

	DWRITE_LINE_METRICS *lineMetrics = nullptr;
	UINT32 numLines = 0;

	cf::MutableArrayRef currentRuns = nullptr; // of type CTRunRef
	cf::MutableArrayRef lines = nullptr; // of type CTLineRef

	bool linesOwnLayout = false; // only used for single line mode, so it maintains an owned reference to layout

	// for DrawGlyphRun callback
	int mLineIndex = 0;
	int mCharIndex = 0;
	dl_CGFloat currentLineWidth = 0;
public:
	// frame-related metrics (that the CTLines still need access to)
	// need to be less redundant about this (provide accessor methods / parent frame prolly)
	DWRITE_TEXT_METRICS textMetrics;
	DWRITE_LINE_SPACING_METHOD lineSpacingMethod;
	FLOAT lineSpacing;
	FLOAT baseline;
	bool rightToLeft = false;

	DWriteLayout(cf::AttributedStringRef attrString);
	~DWriteLayout();

	void currentRunsToLine();

	void prescan();

	// these both call prescan() to perform layout
	// caller owns the result!
	CTLineRef layoutSingleLine();
	cf::ArrayRef layoutArea(dl_CGFloat width, dl_CGFloat height); // of type CTLineRef

	IFACEMETHOD(DrawGlyphRun)(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		__in DWRITE_GLYPH_RUN const* glyphRun,
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		IUnknown* clientDrawingEffect);

	void drawAtPosition(CGContextRef context, dl_CGPoint p) {
		auto defaultBrush = context->getColorBrush(0, 0, 0, 1);
		CoreTextRenderer ctr(context, defaultBrush);
		ctr.Draw(layout, (FLOAT)p.x, (FLOAT)p.y);
	}

	IDWriteTextLayout *getRawLayout() { return layout; }
};
