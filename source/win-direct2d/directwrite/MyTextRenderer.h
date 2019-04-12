#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#include "MyUnknown.h"

class MyTextRenderer
	: public MyUnknown, 
	  public IDWriteTextRenderer // already inherits from IDWritePixelSnapping
{
public:
	MyTextRenderer()
		:MyUnknown(__uuidof(IDWriteTextRenderer), __uuidof(IDWritePixelSnapping)) {}
	~MyTextRenderer() override {}

	IFACEMETHOD(IsPixelSnappingDisabled)(
		__maybenull void* clientDrawingContext,
		__out BOOL* isDisabled
		);

	IFACEMETHOD(GetCurrentTransform)(
		__maybenull void* clientDrawingContext,
		__out DWRITE_MATRIX* transform
		);

	IFACEMETHOD(GetPixelsPerDip)(
		__maybenull void* clientDrawingContext,
		__out FLOAT* pixelsPerDip
		);

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

	IFACEMETHOD(DrawInlineObject)(
		__maybenull void* clientDrawingContext,
		FLOAT originX,
		FLOAT originY,
		IDWriteInlineObject* inlineObject,
		BOOL isSideways,
		BOOL isRightToLeft,
		IUnknown* clientDrawingEffect
		);

	// apparently the intricacies of COM force us to redeclare/implement these :(
	// but at least we can just directly call the MyUnknown versions and not have to REALLY implement them
	IFACEMETHOD_(unsigned long, AddRef) ();
	IFACEMETHOD_(unsigned long, Release) ();
	IFACEMETHOD(QueryInterface) (
		IID const& riid,
		void** ppvObject
		);
};
