#pragma once

#include "../opendl.h"

#include <vector>
#include <assert.h>

#include "caching.h"

#include "../../deps/CFMinimal/source/CF/CFTypes.h"

#include <objbase.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <d2d1helper.h>

#include "COMStuff.h"

#define _USE_MATH_DEFINES
#include <math.h>

// globals
extern ID2D1Factory *d2dFactory;
extern IDWriteFactory *writeFactory;
extern IWICImagingFactory *wicFactory;

enum ClipStackItemType {
    ClipStackItemType_Layer,
    ClipStackItemType_AxisAligned
};

struct ClipStackItem {
    ClipStackItemType itemType;
    ID2D1Layer *clipLayer = 0;
};

// Core Graphics draw state:
//- CTM(current transformation matrix)
//- clip region
//- image interpolation quality
//- line width
//- line join
//- miter limit
//- line cap
//- line dash
//- flatness
//- should anti - alias
//- rendering intent
//- fill color space
//- stroke color space
//- fill color
//- stroke color
//- alpha value
//- font
//- font size
//- character spacing
//- text drawing mode
//- shadow parameters
//- the pattern phase
//- the font smoothing parameter
//- blend mode

// Direct2D DrawingStateBlock: antialiasing mode, transform, tags, and text-rendering options
struct DLDrawState {
    ID2D1RenderTarget *target;

    ID2D1DrawingStateBlock *drawingStackBlock;
    std::vector<ClipStackItem> clipStack; // clipping layers + rects, so they can be pulled off in reverse order

                                          // other state not part of drawingStackBlock/clipStack
    ID2D1SolidColorBrush *fillBrush = nullptr;
    ID2D1SolidColorBrush *strokeBrush = nullptr;
    D2D1_STROKE_STYLE_PROPERTIES props;
    ID2D1StrokeStyle *strokeStyle = nullptr;
    dl_CGFloat lineWidth = 1.0;

    DLDrawState(ID2D1RenderTarget *target, DLDrawState *previous)
        : target(target)
    {
        if (!SUCCEEDED(d2dFactory->CreateDrawingStateBlock(&drawingStackBlock))) {
            printf("** failed to create drawing state block\n");
        }

        if (previous) {
            fillBrush = previous->fillBrush;
            strokeBrush = previous->strokeBrush;
            props = previous->props;

            // we DO addref() to strokeStyle, because it's not cached like the solid colors are
            // ... and we want to be able to release them at some point
            strokeStyle = previous->strokeStyle;
            if (strokeStyle) strokeStyle->AddRef();

            lineWidth = previous->lineWidth;
        }
        else {
            // initialize initial state to avoid crashes
            fillBrush = ::getCachedColorBrush(target, 1, 1, 1, 1);
            strokeBrush = ::getCachedColorBrush(target, 0, 0, 0, 1);

            memset(&props, 0, sizeof(props));
            props.dashStyle = D2D1_DASH_STYLE_SOLID;
            d2dFactory->CreateStrokeStyle(&props, nullptr, 0, &strokeStyle);

            lineWidth = 1.0;
        }
    }
    ~DLDrawState() {
        // release strokeStyle (for now, until / if ever it gets cached)
        if (strokeStyle) strokeStyle->Release();

        // unroll clipstack
        while (clipStack.size() > 0) {
            switch (clipStack.back().itemType) {
            case ClipStackItemType_Layer:
                target->PopLayer();
                clipStack.back().clipLayer->Release();
                break;
            case ClipStackItemType_AxisAligned:
                target->PopAxisAlignedClip();
                break;
            default:
                printf("unknown clip stack type\n");
                assert(0);
            }
            clipStack.pop_back();
        }
    }

    void saveDrawState() {
        target->SaveDrawingState(drawingStackBlock);
    }
    void restoreDrawState() {
        target->RestoreDrawingState(drawingStackBlock);
    }
};


void dlPrivateInit();
void dlPrivateShutdown();

// util funcs
inline D2D1_RECT_F d2dRectFromDlRect(dl_CGRect r) {
	return D2D1::RectF((FLOAT)r.origin.x, (FLOAT)r.origin.y, (FLOAT)(r.origin.x + r.size.width), (FLOAT)(r.origin.y + r.size.height));
}

inline D2D1_ELLIPSE d2dEllipseFromDlRect(dl_CGRect r) {
	D2D1_ELLIPSE e;
	auto xrad = r.size.width / 2.0;
	auto yrad = r.size.height / 2.0;
	auto cx = r.origin.x + xrad;
	auto cy = r.origin.y + yrad;
	e.point = D2D1::Point2F((FLOAT)cx, (FLOAT)cy);
	e.radiusX = (FLOAT)xrad;
	e.radiusY = (FLOAT)yrad;
	return e;
}

inline D2D1_ROUNDED_RECT d2dRoundedRectFromDlRect(dl_CGRect r, dl_CGFloat xrad, dl_CGFloat yrad) {
	D2D1_ROUNDED_RECT rr;
	rr.rect = d2dRectFromDlRect(r);
	rr.radiusX = (FLOAT)xrad;
	rr.radiusY = (FLOAT)yrad;
	return rr;
}

inline dl_CGPoint dlPointFromD2DPoint(D2D1_POINT_2F p) {
	return dl_CGPointMake(p.x, p.y);
}

inline dl_CGAffineTransform dlAffineTransformFromD2DMatrix(D2D1::Matrix3x2F mat) {
	dl_CGAffineTransform ret;
	ret.a = mat.m11;
	ret.b = mat.m12;
	ret.c = mat.m21;
	ret.d = mat.m22;
	ret.tx = mat.dx;
	ret.ty = mat.dy;
	return ret;
}

inline D2D1::Matrix3x2F d2dMatrixFromDLAffineTransform(const dl_CGAffineTransform &t) {
	return D2D1::Matrix3x2F((FLOAT)t.a, (FLOAT)t.b, (FLOAT)t.c, (FLOAT)t.d, (FLOAT)t.tx, (FLOAT)t.ty);
}

inline FLOAT radiansToDegrees(dl_CGFloat x) {
	return (FLOAT)((x * 360.0) / (M_PI * 2.0));
}

inline float pointsToDIP(float pointSize) {
	//return (pointSize * 96.0f) / 72.0f;
	return pointSize;
}

inline DWRITE_TEXT_RANGE dwTextRangeFromDLRange(CFRange r) {
	DWRITE_TEXT_RANGE ret;
	ret.startPosition = (UINT32)r.location;
	ret.length = (UINT32)r.length;
	return ret;
}

inline ID2D1TransformedGeometry *getGlyphRunGeometry(
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_GLYPH_RUN const* glyphRun)
{
	// Create the path geometry.
	ID2D1PathGeometry* pPathGeometry = NULL;
	HR(d2dFactory->CreatePathGeometry(
		&pPathGeometry
	));

	// Write to the path geometry using the geometry sink.
	ID2D1GeometrySink* pSink = NULL;
	HR(pPathGeometry->Open(
		&pSink
	));

	// Get the glyph run outline geometries back from DirectWrite and place them within the
	// geometry sink.
	HR(glyphRun->fontFace->GetGlyphRunOutline(
		glyphRun->fontEmSize,
		glyphRun->glyphIndices,
		glyphRun->glyphAdvances,
		glyphRun->glyphOffsets,
		glyphRun->glyphCount,
		glyphRun->isSideways,
		glyphRun->bidiLevel % 2,
		pSink
	));

	// Close the geometry sink
	HR(pSink->Close());
	SafeRelease(&pSink);

	// Initialize a matrix to translate the origin of the glyph run.
	D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
		1.0f, 0.0f,
		0.0f, 1.0f,
		baselineOriginX, baselineOriginY
	);

	// Create the transformed geometry
	ID2D1TransformedGeometry* pTransformedGeometry = NULL;
	HR(d2dFactory->CreateTransformedGeometry(
		pPathGeometry,
		&matrix,
		&pTransformedGeometry
	));
	SafeRelease(&pPathGeometry);

	return pTransformedGeometry;
}
