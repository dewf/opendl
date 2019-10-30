#pragma once

#include "../../../opendl.h"
#include "../../../common/classes/CF/CFTypes.h"

#include <wincodec.h> // for IWICBitmap

#include "CGColor.h"
#include "CGGradient.h"
#include "CGImage.h"
#include "../../private_defs.h"  // OK for now, but eventually move draw state stuff into this file

#include "../../COMStuff.h"
#include "../../caching.h"

class CGContext; typedef CGContext* CGContextRef;
class CGContext : public cf::Object {
	std::vector<DLDrawState *> drawStateStack;

	// things that are not part of draw state:
	std::vector<PathElement> pathElements;
	D2D1::Matrix3x2F textMatrix = D2D1::Matrix3x2F::Identity();
	D2D1_POINT_2F textPosition = D2D1::Point2F(0, 0); // is this part of the draw state? probably not?
	dl_CGTextDrawingMode textDrawingMode = dl_kCGTextFill;

	ID2D1PathGeometry *pathFromElements(D2D1_FIGURE_BEGIN fillType);
	void clipTargetByHalfPlane(dl_CGPoint hp_point, dl_CGPoint hp_vec);
protected:
	ID2D1RenderTarget *target;

public:
	const char *getTypeName() const override { return "CGContext"; }

	CGContext(ID2D1RenderTarget *target)
		:target(target)
	{
		// need to addref??
		// push initial drawstate: must have a current draw state at all times
		drawStateStack.push_back(new DLDrawState(target, nullptr)); // previous = nullptr, initial item (will populate default values for things)
	}
	virtual ~CGContext() override {
		// clean up / apply any remaining drawstate
		// (mainly useful for unrolling/applying the clip stack)
		while (drawStateStack.size() > 0) {
			delete drawStateStack.back();
			drawStateStack.pop_back();
		}
	}

	virtual CGContextRef copy() override {
		throw cf::Exception("CGContexts don't support cf::Object::copy()");
	};

	DLDrawState *drawState() { return drawStateStack.back(); }

	void saveDrawState() {
		drawStateStack.back()->saveDrawState();
		drawStateStack.push_back(new DLDrawState(target, drawStateStack.back())); // copy from previous on stack
	}

	void restoreDrawState() {
		delete drawStateStack.back();
		drawStateStack.pop_back();
		drawStateStack.back()->restoreDrawState();
	}

	// returns the ratio of global matrix / text matrix
	// useful for getting the right line width
	// though I'm not sure how to best "average" the X/Y scaling - for now we'll use geometric mean
	double getTextScaleRatio() {
		D2D1_MATRIX_3X2_F ctm;
		target->GetTransform(&ctm);
		auto globalMean = sqrt(abs(ctm.m11 * ctm.m22));
		auto textMean = sqrt(abs(textMatrix.m11 * textMatrix.m22));
		return globalMean / textMean;
	}

	inline ID2D1SolidColorBrush *getColorBrush(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha) {
		return ::getCachedColorBrush(target, red, green, blue, alpha);
	}

	inline D2D1_POINT_2F getTextPosition() {
		return textPosition;
	}

	void advanceTextPosition(dl_CGFloat amount) {
		// rendering text causes the text position to move - matching OSX behavior
		textPosition.x += (FLOAT)amount;
	}

	// TODO: would like to get rid of this, but it may be more trouble than it's worth for now
	inline ID2D1RenderTarget *__getTarget() {
		return target;
	}

	// internal/API-private text methods =======================================
	bool textModeIsClipping = false;
	std::vector<ID2D1Geometry *> textClipGeoms;

	inline bool textModeDoesClip() {
		switch (textDrawingMode) {
		case dl_kCGTextFillClip:
		case dl_kCGTextStrokeClip:
		case dl_kCGTextFillStrokeClip:
		case dl_kCGTextClip:
			return true;
		}
		return false;
	}

	inline void clearTextClipGeoms() {
		for (auto i = textClipGeoms.begin(); i != textClipGeoms.end(); i++) {
			(*i)->Release();
		}
		textClipGeoms.clear();
	}

	void accumulateTextClip(ID2D1Geometry *geom) {
		// need to accumulate multiple glyph runs into a single geometry before clipping
		// (otherwise the intersections of them would produce no area at all, since they don't overlap)
		geom->AddRef();
		textClipGeoms.push_back(geom);
	}

	void beginTextDraw() {
		// do any prep based on the text drawing mode
		// this is called before rendering an IDWriteTextLayout (one or more glyphs following)
		// multiple calls to drawGlyphRun will (probably) occur after this
		if (textModeDoesClip()) {
			textModeIsClipping = true;
		}
	}

	void endTextDraw() {
		// any post work for text drawing mode, bookending the above
		if (textModeIsClipping) {
			// create a single geom
			ID2D1GeometryGroup *grouped;

			if (textClipGeoms.size() > 0) {
				HR(d2dFactory->CreateGeometryGroup(
					D2D1_FILL_MODE_WINDING, // doesn't really matter, they shouldn't overlap
					textClipGeoms.data(),
					(UINT32)textClipGeoms.size(),
					&grouped));


				// apply
				clipToPath(grouped);
				// not sure if this should wipe out the current path? (clipToPath alone does not)

				// done
				SafeRelease(&grouped);
			}
			clearTextClipGeoms(); // release items + clear vector
			textModeIsClipping = false;
		}
	}

	void drawGlyphRun(
		void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		DWRITE_GLYPH_RUN const* glyphRun,
		DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		IUnknown* clientDrawingEffect,
		// some extra (non IDWriteTextRenderer) params below
		DWRITE_LINE_METRICS *metrics,
		ID2D1SolidColorBrush* pDefaultBrush
	); // defined in cpp file

	inline void commonTextDraw(
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_GLYPH_RUN const* glyphRun,
		ID2D1SolidColorBrush *fillBrush,  // note: fill/stroke param order reversed from old CoreTextRenderer!
		ID2D1SolidColorBrush *strokeBrush,
		FLOAT strokeWidth
	); // text mode switch

	void fillStrokeClip(
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_GLYPH_RUN const* glyphRun,
		ID2D1SolidColorBrush *strokeBrush,
		ID2D1SolidColorBrush *fillBrush,
		FLOAT strokeWidth,
		bool doClip = false
	); // defined in cpp file

	// end text methods ============================================================

	// public OpenDL API methods ================================
	inline void setRGBFillColor(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha)
	{
		drawState()->fillBrush = ::getCachedColorBrush(target, red, green, blue, alpha);
	}

	inline void fillRect(dl_CGRect rect) {
		target->FillRectangle(d2dRectFromDlRect(rect), drawState()->fillBrush);
	}

	inline void setRGBStrokeColor(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha)
	{
		drawState()->strokeBrush = ::getCachedColorBrush(target, red, green, blue, alpha);
	}

	inline void strokeRectWithWidth(dl_CGRect rect, dl_CGFloat width)
	{
		target->DrawRectangle(d2dRectFromDlRect(rect), drawState()->strokeBrush, (FLOAT)width, drawState()->strokeStyle);
		// old implementation used no stroke style ... any reason for that?

		// Quartz docs say: "The current path is cleared as a side effect of calling this function" ... should we do that?
	}

	inline void strokeRect(dl_CGRect rect)
	{
		strokeRectWithWidth(rect, drawState()->lineWidth);
	}

	inline void beginPath() {
		pathElements.clear();
	}

	inline void closePath() {
		PathElement e;
		e.elementType = PathElement_Closure;
		pathElements.push_back(e);
	}

	inline void moveToPoint(dl_CGFloat x, dl_CGFloat y)
	{
		PathElement e;
		e.elementType = PathElement_StartPoint;
		e.point.x = x;
		e.point.y = y;
		pathElements.push_back(e);
	}

	inline void addLineToPoint(dl_CGFloat x, dl_CGFloat y)
	{
		PathElement e;
		e.elementType = PathElement_LineToPoint;
		e.point.x = x;
		e.point.y = y;
		pathElements.push_back(e);
	}

	inline void addRect(dl_CGRect rect)
	{
		PathElement e;
		e.elementType = PathElement_Rect;
		e.rect = rect;
		pathElements.push_back(e);
	}

	inline void addEllipseInRect(dl_CGRect rect) {
		PathElement e;
		e.elementType = PathElement_Ellipse;
		auto halfWidth = rect.size.width / 2;
		auto halfHeight = rect.size.height / 2;
		e.ellipse.point.x = (FLOAT)(rect.origin.x + halfWidth);
		e.ellipse.point.y = (FLOAT)(rect.origin.y + halfHeight);
		e.ellipse.radiusX = (FLOAT)halfWidth;
		e.ellipse.radiusY = (FLOAT)halfHeight;
		pathElements.push_back(e);
	}

	inline void addRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight)
	{
		PathElement e;
		e.elementType = PathElement_RoundedRect;
		e.roundedRect.rect = d2dRectFromDlRect(rect);
		e.roundedRect.radiusX = (FLOAT)cornerWidth;
		e.roundedRect.radiusY = (FLOAT)cornerHeight;
		pathElements.push_back(e);
	}

	inline void addArc(dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, int clockwise)
	{
		PathElement e;
		e.elementType = PathElement_Arc;
		e.arc.x = x;
		e.arc.y = y;
		e.arc.radius = radius;
		e.arc.startAngle = startAngle;
		e.arc.endAngle = endAngle;
		e.arc.clockwise = clockwise;
		pathElements.push_back(e);
	}

	void addArcToPoint(dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius)
	{
		PathElement e;
		e.elementType = PathElement_ArcToPoint;
		e.arcToPoint.x1 = x1;
		e.arcToPoint.y1 = y1;
		e.arcToPoint.x2 = x2;
		e.arcToPoint.y2 = y2;
		e.arcToPoint.radius = radius;
		pathElements.push_back(e);
	}

	inline void setLineWidth(dl_CGFloat width)
	{
		drawState()->lineWidth = width;
	}

	#define MAX_DASH_LENGTHS 1024
	inline void setLineDash(dl_CGFloat phase, const dl_CGFloat *lengths, size_t count)
	{
		static FLOAT floatDashes[MAX_DASH_LENGTHS];
		assert(count <= MAX_DASH_LENGTHS);

		if (drawState()->strokeStyle) {
			// only OK to release these because we're doing AddRef() to them on the stack ...
			// (see DLDrawState internal notes)
			drawState()->strokeStyle->Release();
			drawState()->strokeStyle = nullptr;
		}

		// below we divide the phase and individuals lengths by c->lineWidth,
		// since during drawing these values get multiplied by c->lineWidth

		if (count > 0 && lengths != nullptr) {
			drawState()->props.dashStyle = D2D1_DASH_STYLE_CUSTOM;
			drawState()->props.dashOffset = (FLOAT)(phase / drawState()->lineWidth); // see note above
			for (unsigned int i = 0; i < count; i++) {
				// these need to be floats, not doubles (dl_CGFloat = double)
				// re: division, see note above
				floatDashes[i] = (FLOAT)(lengths[i] / drawState()->lineWidth);
			}
			d2dFactory->CreateStrokeStyle(&drawState()->props, floatDashes, (UINT32)count, &drawState()->strokeStyle);
		}
		else {
			drawState()->props.dashStyle = D2D1_DASH_STYLE_SOLID;
			d2dFactory->CreateStrokeStyle(&drawState()->props, nullptr, 0, &drawState()->strokeStyle);
		}
	}

	inline void drawPath(dl_CGPathDrawingMode mode)
	{
		D2D1_FIGURE_BEGIN fillType;
		switch (mode) {
		case dl_kCGPathFill:
		case dl_kCGPathFillStroke:
			fillType = D2D1_FIGURE_BEGIN_FILLED;
			break;
		case dl_kCGPathStroke:
			fillType = D2D1_FIGURE_BEGIN_HOLLOW;
			break;
		}

		auto currentPath = pathFromElements(fillType); // this is common to drawing path and clip path creation

		if (mode == dl_kCGPathFill || mode == dl_kCGPathFillStroke) {
			target->FillGeometry(currentPath, drawState()->fillBrush);
		}
		if (mode == dl_kCGPathStroke || mode == dl_kCGPathFillStroke) {
			//printf("drawing path with stroke style %08X\n", c->strokeStyle);
			target->DrawGeometry(currentPath, drawState()->strokeBrush, (FLOAT)drawState()->lineWidth, drawState()->strokeStyle);
		}

		SafeRelease(&currentPath);

		pathElements.clear();
	}

	inline void strokePath()
	{
		// really just the same as dl_CGContextDrawPath(c, dl_kCGPathStroke)
		auto currentPath = pathFromElements(D2D1_FIGURE_BEGIN_HOLLOW);
		target->DrawGeometry(currentPath, drawState()->strokeBrush, (FLOAT)drawState()->lineWidth, drawState()->strokeStyle);
		SafeRelease(&currentPath);
		pathElements.clear();
	}

	inline void fillPath() {
		// really just the same as dl_CGContextDrawPath(c, dl_kCGPathFill)
		auto currentPath = pathFromElements(D2D1_FIGURE_BEGIN_FILLED);
		target->FillGeometry(currentPath, drawState()->fillBrush);
		SafeRelease(&currentPath);
		pathElements.clear();
	}

	inline void translateCTM(dl_CGFloat tx, dl_CGFloat ty)
	{
		D2D1_MATRIX_3X2_F m;
		target->GetTransform(&m);
		target->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)tx, (FLOAT)ty) * m);
	}

	inline void rotateCTM(dl_CGFloat angle)
	{
		D2D1::Matrix3x2F m;
		target->GetTransform(&m);
		auto center = D2D1::Point2F(0, 0);
		auto rotation = D2D1::Matrix3x2F::Rotation((FLOAT)((angle * 360.0) / (M_PI * 2)), center);
		target->SetTransform(rotation * m);
	}

	inline void scaleCTM(dl_CGFloat scaleX, dl_CGFloat scaleY)
	{
		D2D1_MATRIX_3X2_F m;
		target->GetTransform(&m);
		auto center = D2D1::Point2F(m.dx, m.dy);
		auto size = D2D1::SizeF((FLOAT)scaleX, (FLOAT)scaleY);
		auto scale = D2D1::Matrix3x2F::Scale(size, center);
		target->SetTransform(m * scale);
	}

	inline void concatCTM(dl_CGAffineTransform transform)
	{
		D2D1_MATRIX_3X2_F oldMat;
		target->GetTransform(&oldMat);
		auto newMat = D2D1::Matrix3x2F((FLOAT)transform.a, (FLOAT)transform.b, (FLOAT)transform.c, (FLOAT)transform.d, (FLOAT)transform.tx, (FLOAT)transform.ty);
		target->SetTransform(newMat * oldMat);
	}

	inline void clipToPath(ID2D1Geometry *path)
	{
		// create layer for pre-windows 8
		// apparently not necessary for 8+?
		ClipStackItem stackItem;
		stackItem.itemType = ClipStackItemType_Layer;
		auto hr = target->CreateLayer(&stackItem.clipLayer);
		target->PushLayer(
			D2D1::LayerParameters(D2D1::InfiniteRect(), path),
			stackItem.clipLayer);

		drawState()->clipStack.push_back(stackItem);
		// end layer
	}

	inline void clipCurrentPath()
	{
		auto currentPath = pathFromElements(D2D1_FIGURE_BEGIN_FILLED); // this is common to drawing path and clip path creation

		clipToPath(currentPath);

		SafeRelease(&currentPath);

		pathElements.clear();
	}

	inline void clipToRect(dl_CGRect rect)
	{
		D2D1::Matrix3x2F m;
		target->GetTransform(&m);
		if (m.IsIdentity()) {
			// can only use axis-aligned clip if there is no transform active,
			// otherwise it's ... not going to work (well, technically if it was only transformed / scaled but not rotated, then it might be OK)
			target->PushAxisAlignedClip(
				d2dRectFromDlRect(rect),
				D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

			ClipStackItem item;
			item.itemType = ClipStackItemType_AxisAligned;
			drawState()->clipStack.push_back(item);
		}
		else {
			// do it the hard way ...
			addRect(rect);
			clipCurrentPath();
		}
	}

	inline void saveGState()
	{
		saveDrawState();
	}

	inline void restoreGState()
	{
		restoreDrawState();
	}

	inline void setFillColorWithColor(dl_CGColorRef color)
	{
		drawState()->fillBrush = ((CGColorRef)color)->getCachedBrush(target);
	}

	inline void setStrokeColorWithColor(dl_CGColorRef color)
	{
		drawState()->strokeBrush = ((CGColorRef)color)->getCachedBrush(target);
	}

	void fillViewportWithBrush(ID2D1LinearGradientBrush * gradBrush)
	{
		// must take into account that we might have a current transform, in which case Rect2F(0, 0, width, height) will no longer work properly
		// create inverse transform and acquire viewport corners that way, sigh
		D2D1::Matrix3x2F xform;
		target->GetTransform(&xform);
		xform.Invert();
		auto size = target->GetSize();
		auto p0 = xform.TransformPoint(D2D1::Point2F(0, 0));
		auto p1 = xform.TransformPoint(D2D1::Point2F(size.width, 0));
		auto p2 = xform.TransformPoint(D2D1::Point2F(size.width, size.height));
		auto p3 = xform.TransformPoint(D2D1::Point2F(0, size.height));
		beginPath();
		moveToPoint(p0.x, p0.y);
		addLineToPoint(p1.x, p1.y);
		addLineToPoint(p2.x, p2.y);
		addLineToPoint(p3.x, p3.y);
		closePath();

		auto currentPath = pathFromElements(D2D1_FIGURE_BEGIN_FILLED);
		target->FillGeometry(currentPath, gradBrush);
		SafeRelease(&currentPath);
		pathElements.clear();
	}

	inline void drawLinearGradient(dl_CGGradientRef gradient, dl_CGPoint startPoint, dl_CGPoint endPoint, dl_CGGradientDrawingOptions options)
	{
		HRESULT hr;

		ID2D1GradientStopCollection *gradStopCollection;
		hr = target->CreateGradientStopCollection(
			((CGGradientRef)gradient)->getStops(), 
			(UINT32)((CGGradientRef)gradient)->getStopCount(), 
			&gradStopCollection);
		assert(SUCCEEDED(hr));

		ID2D1LinearGradientBrush *gradBrush;
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props;
		props.startPoint = D2D1::Point2F((FLOAT)startPoint.x, (FLOAT)startPoint.y);
		props.endPoint = D2D1::Point2F((FLOAT)endPoint.x, (FLOAT)endPoint.y);
		hr = target->CreateLinearGradientBrush(props, gradStopCollection, &gradBrush);
		assert(SUCCEEDED(hr));

		// save because we're about to create a clip mask
		saveGState();

		dl_CGPoint vec = { endPoint.x - startPoint.x, endPoint.y - startPoint.y };
		if (!(options & dl_kCGGradientDrawsBeforeStartLocation)) {
			clipTargetByHalfPlane({ startPoint.x, startPoint.y }, vec);
		}
		if (!(options & dl_kCGGradientDrawsAfterEndLocation)) {
			dl_CGPoint vec2 = { -vec.x, -vec.y };
			clipTargetByHalfPlane({ endPoint.x, endPoint.y }, vec2);
		}

		// fill the entire viewport, because the gradient might extend beyond the start/end points
		// (will be clipped by half planes above if not)
		fillViewportWithBrush(gradBrush);

		//target->FillRectangle(r2, gradBrush);

		// end clip mask
		restoreGState();

		gradBrush->Release();
		gradStopCollection->Release();
	}

	inline void setTextMatrix(dl_CGAffineTransform t)
	{
		textMatrix = d2dMatrixFromDLAffineTransform(t);
	}

	inline void setTextPosition(dl_CGFloat x, dl_CGFloat y)
	{
		textPosition = D2D1::Point2F((FLOAT)x, (FLOAT)y);
	}

	inline void setTextDrawingMode(dl_CGTextDrawingMode mode)
	{
		textDrawingMode = mode;
		// anything else?
	}

	inline void clipToMask(dl_CGRect rect, CGImageRef mask)
	{
		// D2D uses alpha for masking, but Quartz uses grayscale images
		// this is our approximation for now
		auto alphaBitmap = mask->getAlphaBitmapFromLuminance(target);

		ID2D1BitmapBrush *bitmapBrush;
		target->CreateBitmapBrush(alphaBitmap, &bitmapBrush);
		alphaBitmap->Release();

		// use bitmapBrush as alpha mask
		// push clip mask
		ClipStackItem stackItem2;
		stackItem2.itemType = ClipStackItemType_Layer;
		target->CreateLayer(&stackItem2.clipLayer);
		target->PushLayer(
			D2D1::LayerParameters(
				D2D1::InfiniteRect(),
				NULL,
				D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
				D2D1::IdentityMatrix(),
				1.0F,
				bitmapBrush),
			stackItem2.clipLayer);
		drawState()->clipStack.push_back(stackItem2);

		bitmapBrush->Release();
	}

	inline void drawImage(dl_CGRect rect, CGImageRef image) {
		UINT width, height;
		image->getSize(&width, &height);
		auto sourceRect = D2D1::RectF(0, 0, (FLOAT)width, (FLOAT)height);

		auto destRect = d2dRectFromDlRect(rect);

		auto bitmap = image->getBitmap(target);

		target->DrawBitmap(bitmap, destRect, 1.0,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			sourceRect);

		bitmap->Release();
	}
};
