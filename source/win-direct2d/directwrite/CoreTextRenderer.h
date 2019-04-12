#pragma once
#include "MyTextRenderer.h"

#include "CoreTextFormatSpec.h"
#include "../../opendl.h"

#include "../classes/CG/CGContext.h"

class CoreTextRenderer : public MyTextRenderer
{
private:
	DWRITE_LINE_METRICS *mLineMetrics = nullptr;
	UINT32 mNumLines = 0;
	UINT32 mCharIndex = 0;
	UINT32 mLineIndex = 0;

	double textScaleRatio = 1.0;

	CGContextRef context;
	ID2D1SolidColorBrush *pDefaultBrush;

	// private methods after public below
public:
	CoreTextRenderer(CGContextRef context, ID2D1SolidColorBrush* pDefaultBrush)
		:context(context),
		 textScaleRatio(context->getTextScaleRatio()),
		 pDefaultBrush(pDefaultBrush)
	{
		if (pDefaultBrush) {
			pDefaultBrush->AddRef();
		}
	}
	
	virtual ~CoreTextRenderer() override {
		SafeRelease(&pDefaultBrush);
	}

	IFACEMETHOD(DrawGlyphRun)(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		__in DWRITE_GLYPH_RUN const* glyphRun,
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		IUnknown* clientDrawingEffect
		);

	IFACEMETHOD(DrawUnderline)(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_UNDERLINE const* underline,
		IUnknown* clientDrawingEffect
		);

	IFACEMETHOD(DrawStrikethrough)(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_STRIKETHROUGH const* strikethrough,
		IUnknown* clientDrawingEffect
		);

	void Draw(IDWriteTextLayout *layout, FLOAT x, FLOAT y);

//private:
//	// more private stuff
//	void fillStrokeGlyphRun(
//		__maybenull void* clientDrawingContext,
//		FLOAT baselineOriginX,
//		FLOAT baselineOriginY,
//		DWRITE_MEASURING_MODE measuringMode,
//		__in DWRITE_GLYPH_RUN const* glyphRun,
//		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
//		ID2D1SolidColorBrush *strokeBrush,
//		ID2D1SolidColorBrush *fillBrush,
//		FLOAT strokeWidth);
};

