#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include <vector>

#include <assert.h>

#include <utility>
#include <math.h>

#include "../../private_defs.h"
#include "../../../common/geometry.h" // for calcArcToPoint

#include "../../../common/pathstuff.h"

// PathSegment methods =================================================================

static inline void geomSinkAddArc(PathSegment &seg, ID2D1GeometrySink *geomSink, D2D1_FIGURE_BEGIN fillType, bool &figureOpen, D2D1_POINT_2F &figureBeginPoint, D2D1_POINT_2F &lastPoint)
{
	dl_CGFloat clockMul = seg.arc.clockwise ? 1.0 : -1.0;
	dl_CGFloat p1x = seg.arc.x + (cos(clockMul * seg.arc.startAngle) * seg.arc.radius);
	dl_CGFloat p1y = seg.arc.y + (sin(clockMul * seg.arc.startAngle) * seg.arc.radius);
	auto p1 = D2D1::Point2F((FLOAT)p1x, (FLOAT)p1y);
	if (figureOpen) {
		// already in a figure - line to start of arc
		geomSink->AddLine(p1);
	}
	else {
		// figure not yet started - move to start of arc
		geomSink->BeginFigure(p1, fillType);
		figureBeginPoint = p1;
		figureOpen = true;
	}

	// this would probably be better to do with epsilon / c++ numeric limits stuff
	// basically make sure they're not equal (after an fmod(M_PI*2)), otherwise windows puts 360-degree circles in the wrong place
	auto diff = fmod(abs(seg.arc.endAngle - seg.arc.startAngle), M_PI * 2.0);
	bool fullCircle = diff < 0.0001;
	if (fullCircle) {
		seg.arc.endAngle *= 0.9999;
		// and we'll have to manually EndFigure because otherwise there'd be a gap in the stroke
	}

	dl_CGFloat p2x = seg.arc.x + (cos(clockMul * seg.arc.endAngle) * seg.arc.radius);
	dl_CGFloat p2y = seg.arc.y + (sin(clockMul * seg.arc.endAngle) * seg.arc.radius);
	auto p2 = D2D1::Point2F((FLOAT)p2x, (FLOAT)p2y);

	auto size = D2D1::SizeF((FLOAT)seg.arc.radius, (FLOAT)seg.arc.radius);
	auto dir = seg.arc.clockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
	auto angle = ((seg.arc.endAngle - seg.arc.startAngle) * 360.0) / (M_PI * 2.0);
	auto big = (angle >= 180.0) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
	auto arc = D2D1::ArcSegment(p2, size, 0, dir, big);
	geomSink->AddArc(arc);

	if (fullCircle) {
		// close with line to p1 (arc begin)
		geomSink->AddLine(p1);
		lastPoint = p1;
		//geomSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		//*figureOpen = false;
	}
	else {
		// only a semicircle, use p2 as lastpoint instead
		lastPoint = p2;
	}
}

static inline void geomSinkAddArcToPoint(PathSegment &seg, ID2D1GeometrySink *geomSink, D2D1_POINT_2F *lastPoint)
{
	double centerX, centerY, startX, startY, angle0, angle1;

	calcArcToPoint(
		lastPoint->x, lastPoint->y,
		seg.arcToPoint.x1, seg.arcToPoint.y1,
		seg.arcToPoint.x2, seg.arcToPoint.y2,
		seg.arcToPoint.radius,
		&centerX, &centerY, &startX, &startY, &angle0, &angle1);

	// draw the damn thing
	geomSink->AddLine(D2D1::Point2F((FLOAT)startX, (FLOAT)startY));
	auto endX = centerX + cos(angle1) * seg.arcToPoint.radius;
	auto endY = centerY + sin(angle1) * seg.arcToPoint.radius;
	auto endPoint = D2D1::Point2F((FLOAT)endX, (FLOAT)endY);
	auto arc = D2D1::ArcSegment(
		endPoint,
		D2D1::SizeF((FLOAT)seg.arcToPoint.radius, (FLOAT)seg.arcToPoint.radius),
		0,
		D2D1_SWEEP_DIRECTION_CLOCKWISE,
		(angle1 - angle0) > M_PI ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL);
	geomSink->AddArc(arc);
	// we end at the end of that arc, resume drawing from there
	*lastPoint = endPoint;
}

static inline void checkBeginFigure(ID2D1GeometrySink *geomSink, D2D1_FIGURE_BEGIN fillType, bool &figureOpen, D2D1_POINT_2F &figureBeginPoint, D2D1_POINT_2F &lastPoint) {
	if (!figureOpen) {
		geomSink->BeginFigure(lastPoint, fillType);
		figureBeginPoint = lastPoint;
		figureOpen = true;
	}
}

static inline void writeToSink(PathSegment &seg, ID2D1GeometrySink *geomSink, D2D1_FIGURE_BEGIN fillType, bool &figureOpen, D2D1_POINT_2F &figureBeginPoint, D2D1_POINT_2F &lastPoint) {
	switch (seg.tag) {
	case PathSegment::Tag_MoveToPoint: {
		if (figureOpen) {
			// terminate whatever figure was in progress ...
			geomSink->EndFigure(D2D1_FIGURE_END_OPEN);
			figureOpen = false;
		}
		lastPoint = D2D1::Point2F((FLOAT)seg.point.x, (FLOAT)seg.point.y);
		break;
	}
	case PathSegment::Tag_LineToPoint: {
		checkBeginFigure(geomSink, fillType, figureOpen, figureBeginPoint, lastPoint);
		auto p = D2D1::Point2F((FLOAT)seg.point.x, (FLOAT)seg.point.y);
		geomSink->AddLine(p);
		lastPoint = p;
		break;
	}
	case PathSegment::Tag_Arc: {
		// no checkBeginFigure() - arc handling is more complex, that logic is handled internally
		geomSinkAddArc(seg, geomSink, fillType, figureOpen, figureBeginPoint, lastPoint);
		break;
	}
	case PathSegment::Tag_ArcToPoint: {
		checkBeginFigure(geomSink, fillType, figureOpen, figureBeginPoint, lastPoint);
		geomSinkAddArcToPoint(seg, geomSink, &lastPoint); // updates lastPoint
		break;
	}
	case PathSegment::Tag_CurveToPoint: {
		checkBeginFigure(geomSink, fillType, figureOpen, figureBeginPoint, lastPoint);
		D2D1_BEZIER_SEGMENT bez;
		bez.point1 = D2D1::Point2F((FLOAT)seg.curveToPoint.cp1x, (FLOAT)seg.curveToPoint.cp1y);
		bez.point2 = D2D1::Point2F((FLOAT)seg.curveToPoint.cp2x, (FLOAT)seg.curveToPoint.cp2y);
		lastPoint = D2D1::Point2F((FLOAT)seg.curveToPoint.x, (FLOAT)seg.curveToPoint.y);
		bez.point3 = lastPoint;
		geomSink->AddBezier(bez);
		break;
	}
	case PathSegment::Tag_QuadCurveToPoint: {
		checkBeginFigure(geomSink, fillType, figureOpen, figureBeginPoint, lastPoint);
		D2D1_QUADRATIC_BEZIER_SEGMENT quad;
		quad.point1 = D2D1::Point2F((FLOAT)seg.quadCurveToPoint.cpx, (FLOAT)seg.quadCurveToPoint.cpy);
		lastPoint = D2D1::Point2F((FLOAT)seg.quadCurveToPoint.x, (FLOAT)seg.quadCurveToPoint.y);
		quad.point2 = lastPoint;
		geomSink->AddQuadraticBezier(quad);
		break;
	}
	case PathSegment::Tag_Closure: {
		if (figureOpen) {
			geomSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			figureOpen = false;
			lastPoint = figureBeginPoint;
		}
		break;
	}
	default:
		printf("mkSegmentedGeom: unhandled switch case\n");
		assert(false);
	} // end switch
}

// SubPath methods ======================================================

static ID2D1Geometry *maybeTransformedGeom(SubPath &sp, ID2D1Geometry *input) {
	if (sp.hasTransform) {
		ID2D1TransformedGeometry *xformed;
		auto matrix = d2dMatrixFromDLAffineTransform(sp.transform);
		HR(d2dFactory->CreateTransformedGeometry(input, matrix, &xformed));
		input->Release();
		return xformed;
	}
	else {
		return input;
	}
}

static ID2D1Geometry *mkRectGeom(SubPath &sp) {
	ID2D1RectangleGeometry *geom;
	auto d2dRect = d2dRectFromDlRect(sp.rect.value);
	HR(d2dFactory->CreateRectangleGeometry(d2dRect, &geom));
	return maybeTransformedGeom(sp, geom);
}

static ID2D1Geometry *mkEllipseGeom(SubPath &sp) {
	ID2D1EllipseGeometry *geom;
	auto d2dEllipse = d2dEllipseFromDlRect(sp.ellipse.inRect);
	HR(d2dFactory->CreateEllipseGeometry(d2dEllipse, &geom));
	return maybeTransformedGeom(sp, geom);
}

static ID2D1Geometry *mkRoundedRectGeom(SubPath &sp) {
	ID2D1RoundedRectangleGeometry *geom;
	auto d2dRounded = d2dRoundedRectFromDlRect(sp.rounded.rect, sp.rounded.cornerWidth, sp.rounded.cornerWidth);
	HR(d2dFactory->CreateRoundedRectangleGeometry(d2dRounded, &geom));
	return maybeTransformedGeom(sp, geom);
}

static ID2D1Geometry *mkSegmentedGeom(SubPath &sp, D2D1_FIGURE_BEGIN fillType) {
	ID2D1PathGeometry *geom;
	ID2D1GeometrySink *geomSink;
	HR(d2dFactory->CreatePathGeometry(&geom));
	HR(geom->Open(&geomSink));
	geomSink->SetFillMode(D2D1_FILL_MODE_WINDING);

	bool figureOpen = false;
	D2D1_POINT_2F figureBeginPoint, lastPoint;

	// write out each element to the geom sink
	for (auto i = sp.segments.begin(); i != sp.segments.end(); i++) {
		writeToSink(*i, geomSink, fillType, figureOpen, figureBeginPoint, lastPoint);
	}

	if (figureOpen) {
		geomSink->EndFigure(D2D1_FIGURE_END_OPEN);
	}

	geomSink->Close();
	geomSink->Release();
	return maybeTransformedGeom(sp, geom);
}

static ID2D1Geometry *getGeometry(SubPath &sp, D2D1_FIGURE_BEGIN fillType) {
	switch (sp.tag) {
	case SubPath::Tag_Rect:
		return mkRectGeom(sp);
	case SubPath::Tag_Ellipse:
		return mkEllipseGeom(sp);
	case SubPath::Tag_RoundedRect:
		return mkRoundedRectGeom(sp);
	case SubPath::Tag_Segmented:
		return mkSegmentedGeom(sp, fillType);
	}
	// should never get here
	return nullptr;
}

// CGPath / CGMutablePath proper =========================================================================

class CGPath; typedef CGPath* CGPathRef;
class CGPath : public cf::Object {
protected:
	std::vector<SubPath> subPaths;

	SubPath *currentSub() {
		return &subPaths[subPaths.size() - 1];
	}

public:
	const char *getTypeName() const override { return "CGPath"; }

	CGPath() {}

	CGPath(SubPath sub) {
		subPaths.push_back(sub);
		// todo: update currentPoint with end of SubPath
	}

	CGPath(const CGPath &other) {
		subPaths = other.subPaths;
		// todo: update currentPoint with end of last SubPath
	}

	static CGPathRef createWithRect(dl_CGRect &rect, const dl_CGAffineTransform *transform) {
		return new CGPath(SubPath::createRect(rect, transform));
	}

	static CGPathRef createWithEllipseInRect(dl_CGRect &rect, const dl_CGAffineTransform *transform) {
		return new CGPath(SubPath::createEllipse(rect, transform));
	}

	static CGPathRef createWithRoundedRect(dl_CGRect &rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform) {
		return new CGPath(SubPath::createRoundedRect(rect, cornerWidth, cornerHeight, transform));
	}

	bool isRect() {
		// kind of hacky at the moment, no mechanism in place to prevent adding further subpaths
		return
			subPaths.size() == 1 &&
			currentSub()->isRect();
	}

	dl_CGRect getRect() {
		if (isRect()) {
			return currentSub()->getRect();
		}
		else {
			throw cf::Exception("CGPath was not a rect");
		}
	}

	virtual CGPathRef copy() {
		// immutable so return self
		return (CGPathRef)retain();
	}

	// for CGMutablePaths accessing needing access to another CGPath's content
	std::vector<SubPath>& getSubPaths() {
		return subPaths;
	}

	std::vector<ID2D1Geometry*> getGeometry(D2D1_FIGURE_BEGIN fillType) {
		std::vector<ID2D1Geometry*> ret;
		for (auto i = subPaths.begin(); i != subPaths.end(); i++) {
			ret.push_back(::getGeometry(*i, fillType));
		}
		return ret;
	}

	dl_CGPoint getCurrentPoint() {
		return currentSub()->endPoint;
	}

	RETAIN_AND_AUTORELEASE(CGPath)
};


class CGMutablePath; typedef CGMutablePath* CGMutablePathRef;
class CGMutablePath : public CGPath {
private:
	bool open = false; // is there a segmented path on the stack, ready to go?

	inline SubPath *mutableSub() {
		if (!open) {
			subPaths.push_back(SubPath::createEmptyPath());
			open = true;
		}
		return currentSub();
	}
public:
	CGMutablePath() : CGPath() {}

	CGMutablePath(const CGPath *other) // aka create mutable copy
		: CGPath(*other) // copy all previous subpaths
	{}

	~CGMutablePath() {}

	virtual bool operator <(const Object &b) const {
		throw cf::Exception("operator < not defined for CGMutablePath");
	}
	virtual CGPathRef copy() {
		// make an immutable copy
		return new CGPath(*this);
	}

	void moveToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
		mutableSub()->moveToPoint(m, x, y);
	}
	void addArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise) {
		mutableSub()->addArc(m, x, y, radius, startAngle, endAngle, clockwise);
	}
	void addRelativeArc(const dl_CGAffineTransform *matrix, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta) {
		//mutableSub()->addRelativeArc(matrix, x, y, radius, startAngle, delta);
		bool clockwise;
		dl_CGFloat endAngle;
		if (delta >= 0) {
			clockwise = true;
			endAngle = fmod(startAngle + delta, M_PI * 2.0);
		}
		else {
			clockwise = false;
			endAngle = fmod(startAngle + M_PI * 2.0 - delta, M_PI * 2.0);
		}
		mutableSub()->addArc(matrix, x, y, radius, startAngle, endAngle, clockwise);
	}
	void addArcToPoint(const dl_CGAffineTransform *m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius) {
		mutableSub()->addArcToPoint(m, x1, y1, x2, y2, radius);
	}
	void addCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y) {
		mutableSub()->addCurveToPoint(m, cp1x, cp1y, cp2x, cp2y, x, y);
	}
	void addLineToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
		mutableSub()->addLineToPoint(m, x, y);
	}
	void addLines(const dl_CGAffineTransform *m, const dl_CGPoint *points, size_t count) {
		mutableSub()->addLines(m, points, count);
	}
	void addQuadCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y) {
		mutableSub()->addQuadCurveToPoint(m, cpx, cpy, x, y);
	}
	void addRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
		open = false;
		subPaths.push_back(SubPath::createRect(rect, m));
	}
	void addRects(const dl_CGAffineTransform *m, const dl_CGRect *rects, size_t count) {
		open = false;
		for (size_t i = 0; i < count; i++) {
			subPaths.push_back(SubPath::createRect(rects[i], m));
		}
	}
	void addRoundedRect(const dl_CGAffineTransform *transform, dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight) {
		open = false;
		subPaths.push_back(SubPath::createRoundedRect(rect, cornerWidth, cornerHeight, transform));
	}
	void addEllipseInRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
		open = false;
		subPaths.push_back(SubPath::createEllipse(rect, m));
		// TODO: currentpoint should be ... ?
	}
	void addPath(const dl_CGAffineTransform *m, CGPathRef path2) {
		open = false;
		auto sourceSubs = path2->getSubPaths();
		for (auto i = sourceSubs.begin(); i != sourceSubs.end(); i++) {
			SubPath sp = (*i);
			if (m) {
				sp.concatTransform(*m);
			}
			subPaths.push_back(std::move(sp));
		}
	}
	void closeSubpath() {
		mutableSub()->close();
		open = false; // will need to push a new segmented subpath to add further segments (mutableSub() takes care of it)
	}
	void reset() {
		subPaths.clear();
		open = false;
	}

	RETAIN_AND_AUTORELEASE(CGMutablePath)
};
