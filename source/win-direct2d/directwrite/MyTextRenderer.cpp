#include "MyTextRenderer.h"

IFACEMETHODIMP MyTextRenderer::DrawGlyphRun(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode,
	__in DWRITE_GLYPH_RUN const* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
	IUnknown* clientDrawingEffect
)
{
	return S_OK;
}

IFACEMETHODIMP MyTextRenderer::DrawUnderline(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	__in DWRITE_UNDERLINE const* underline,
	IUnknown* clientDrawingEffect
)
{
	return S_OK;
}

IFACEMETHODIMP MyTextRenderer::DrawStrikethrough(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	__in DWRITE_STRIKETHROUGH const* strikethrough,
	IUnknown* clientDrawingEffect
)
{
	return S_OK;
}

IFACEMETHODIMP MyTextRenderer::DrawInlineObject(
	__maybenull void* clientDrawingContext,
	FLOAT originX,
	FLOAT originY,
	IDWriteInlineObject* inlineObject,
	BOOL isSideways,
	BOOL isRightToLeft,
	IUnknown* clientDrawingEffect
)
{
	return S_OK;
}

IFACEMETHODIMP MyTextRenderer::IsPixelSnappingDisabled(
	__maybenull void* clientDrawingContext,
	__out BOOL* isDisabled
)
{
	*isDisabled = FALSE;
	return S_OK;
}

IFACEMETHODIMP MyTextRenderer::GetCurrentTransform(
	__maybenull void* clientDrawingContext,
	__out DWRITE_MATRIX* transform
)
{
	// until we hear diffently from an override ...
	*((D2D1_MATRIX_3X2_F*)transform) = D2D1::Matrix3x2F::Identity();
	return S_OK;
}

IFACEMETHODIMP MyTextRenderer::GetPixelsPerDip(
	__maybenull void* clientDrawingContext,
	__out FLOAT* pixelsPerDip
)
{
	*pixelsPerDip = 1.0f; // until we hear differently ...
	return S_OK;
}

// sigh, annoying to have to implement these since they're already a part of MyUnknown
IFACEMETHODIMP MyTextRenderer::QueryInterface(IID const& riid, void** ppvObject)
{
	return MyUnknown::QueryInterface(riid, ppvObject);
}

IFACEMETHODIMP_(unsigned long) MyTextRenderer::AddRef()
{
	return MyUnknown::AddRef();
}

IFACEMETHODIMP_(unsigned long) MyTextRenderer::Release()
{
	return MyUnknown::Release();
}

