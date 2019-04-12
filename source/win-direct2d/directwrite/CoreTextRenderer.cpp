#include "CoreTextRenderer.h"

#include "CoreTextFormatSpec.h"
#include "..\COMStuff.h"

#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

void CoreTextRenderer::Draw(IDWriteTextLayout *layout, FLOAT x, FLOAT y)
{
	layout->GetLineMetrics(NULL, 0, &mNumLines);
	mLineMetrics = new DWRITE_LINE_METRICS[mNumLines];
	HR(layout->GetLineMetrics(mLineMetrics, mNumLines, &mNumLines));

	mCharIndex = 0;
	mLineIndex = 0;

	context->beginTextDraw();
	layout->Draw(NULL, this, x, y);
	context->endTextDraw();

	delete[] mLineMetrics;
}

IFACEMETHODIMP CoreTextRenderer::DrawGlyphRun(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode,
	__in DWRITE_GLYPH_RUN const* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
	IUnknown* clientDrawingEffect)
{
	auto lineMetrics = mLineMetrics[mLineIndex];
	bool inTrailingWhitespace = (mCharIndex >= (lineMetrics.length - lineMetrics.trailingWhitespaceLength));

	context->drawGlyphRun(
		clientDrawingContext,
		baselineOriginX,
		baselineOriginY,
		measuringMode,
		glyphRun,
		glyphRunDescription,
		clientDrawingEffect,
		&lineMetrics,
		pDefaultBrush);

	mCharIndex += glyphRunDescription->stringLength;
	if (mCharIndex == lineMetrics.length) {
		mLineIndex++;
		mCharIndex = 0;
	}

	return S_OK;
}

IFACEMETHODIMP CoreTextRenderer::DrawUnderline(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	__in DWRITE_UNDERLINE const* underline,
	IUnknown* clientDrawingEffect
)
{
	return S_OK;
}

IFACEMETHODIMP CoreTextRenderer::DrawStrikethrough(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	__in DWRITE_STRIKETHROUGH const* strikethrough,
	IUnknown* clientDrawingEffect
)
{
	return S_OK;
}

//void CoreTextRenderer::fillStrokeGlyphRun(
//	__maybenull void* clientDrawingContext,
//	FLOAT baselineOriginX,
//	FLOAT baselineOriginY,
//	DWRITE_MEASURING_MODE measuringMode,
//	__in DWRITE_GLYPH_RUN const* glyphRun,
//	__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
//	ID2D1SolidColorBrush *strokeBrush,
//	ID2D1SolidColorBrush *fillBrush,
//	FLOAT strokeWidth)
//{
//	auto target = context->__getTarget();
//
//	// Create the path geometry.
//	ID2D1PathGeometry* pPathGeometry = NULL;
//	HR(d2dFactory->CreatePathGeometry(
//		&pPathGeometry
//	));
//
//	// Write to the path geometry using the geometry sink.
//	ID2D1GeometrySink* pSink = NULL;
//	HR(pPathGeometry->Open(
//		&pSink
//	));
//
//	// Get the glyph run outline geometries back from DirectWrite and place them within the
//	// geometry sink.
//	HR(glyphRun->fontFace->GetGlyphRunOutline(
//		glyphRun->fontEmSize,
//		glyphRun->glyphIndices,
//		glyphRun->glyphAdvances,
//		glyphRun->glyphOffsets,
//		glyphRun->glyphCount,
//		glyphRun->isSideways,
//		glyphRun->bidiLevel % 2,
//		pSink
//	));
//
//	// Close the geometry sink
//	HR(pSink->Close());
//
//	// Initialize a matrix to translate the origin of the glyph run.
//	D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
//		1.0f, 0.0f,
//		0.0f, 1.0f,
//		baselineOriginX, baselineOriginY
//	);
//
//	// Create the transformed geometry
//	ID2D1TransformedGeometry* pTransformedGeometry = NULL;
//	HR(d2dFactory->CreateTransformedGeometry(
//		pPathGeometry,
//		&matrix,
//		&pTransformedGeometry
//	));
//
//	if (fillBrush) {
//		// Fill in the glyph run
//		target->FillGeometry(
//			pTransformedGeometry,
//			fillBrush
//		);
//	}
//
//	if (strokeBrush) {
//		// Draw the outline of the glyph run
//		target->DrawGeometry(
//			pTransformedGeometry,
//			strokeBrush,
//			strokeWidth
//		);
//	}
//
//	SafeRelease(&pPathGeometry);
//	SafeRelease(&pSink);
//	SafeRelease(&pTransformedGeometry);
//}
