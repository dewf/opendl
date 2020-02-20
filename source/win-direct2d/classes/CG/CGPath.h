#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include <vector>

#include <utility>

struct PathSegment {
	enum TagEnum {
		Tag_StartPoint,
		Tag_LineToPoint,
		Tag_Arc,
		Tag_RelativeArc,
		Tag_ArcToPoint,
		Tag_CurveToPoint,
		Tag_QuadCurveToPoint,
		Tag_Closure
	} tag;
	union {
		struct {
			dl_CGFloat x, y;
		} point; // StartPoint, LineToPoint

		struct {
			dl_CGFloat x, y, radius, startAngle, endAngle;
			int clockwise;
			dl_CGFloat delta; // used instead of endAngle/clockwise
		} arc; // Arc, RelativeArc

		struct {
			dl_CGFloat x1, y1, x2, y2, radius;
		} arcToPoint;

		struct {
			dl_CGFloat cp1x, cp1y, cp2x, cp2y, x, y;
		} curveToPoint;

		struct {
			dl_CGFloat cpx, cpy, x, y;
		} quadCurveToPoint;
	};
	PathSegment(TagEnum tag) {
		this->tag = tag;
	}
	static PathSegment mkMoveToPoint(dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_StartPoint);
		seg.point.x = x;
		seg.point.y = y;
		return std::move(seg);
	}
	static PathSegment mkLineToPoint(dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_LineToPoint);
		seg.point.x = x;
		seg.point.y = y;
		return std::move(seg);
	}
	static PathSegment mkArc(dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, dl_CGFloat clockwise) {
		PathSegment seg(Tag_Arc);
		seg.arc.x = x;
		seg.arc.y = y;
		seg.arc.radius = radius;
		seg.arc.startAngle = startAngle;
		seg.arc.endAngle = endAngle;
		seg.arc.clockwise = clockwise;
		return std::move(seg);
	}
	static PathSegment mkRelativeArc(dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta) {
		PathSegment seg(Tag_RelativeArc);
		seg.arc.x = x;
		seg.arc.y = y;
		seg.arc.radius = radius;
		seg.arc.startAngle = startAngle;
		seg.arc.delta = delta;
		return std::move(seg);
	}
	static PathSegment mkArcToPoint(dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius) {
		PathSegment seg(Tag_ArcToPoint);
		seg.arcToPoint.x1 = x1;
		seg.arcToPoint.y1 = y1;
		seg.arcToPoint.x2 = x2;
		seg.arcToPoint.y2 = y2;
		seg.arcToPoint.radius = radius;
		return std::move(seg);
	}
	static PathSegment mkCurveToPoint(dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_CurveToPoint);
		seg.curveToPoint.cp1x = cp1x;
		seg.curveToPoint.cp1y = cp1y;
		seg.curveToPoint.cp2x = cp2x;
		seg.curveToPoint.cp2y = cp2y;
		seg.curveToPoint.x = x;
		seg.curveToPoint.y = y;
		return std::move(seg);
	}
	static PathSegment mkQuadCurveToPoint(dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_QuadCurveToPoint);
		seg.quadCurveToPoint.cpx = cpx;
		seg.quadCurveToPoint.cpy = cpy;
		seg.quadCurveToPoint.x = x;
		seg.quadCurveToPoint.y = y;
		return std::move(seg);
	}
	static PathSegment mkClosure() {
		PathSegment seg(Tag_Closure);
		return std::move(seg);
	}
};

struct SubPath {
	enum SubPathTag {
		Tag_Rect,
		Tag_Ellipse,
		Tag_RoundedRect,
		Tag_Segmented
	} tag;

	bool hasTransform = false;
	dl_CGAffineTransform transform;		// current limitation: all items of a given subpath must share same transform
										// (whereas the quartz methods allow each segment to have its own)

	std::vector<PathSegment> segments;  // really belongs in the path struct but that causes C++ issues so ... here instead

	union {
		struct {
			dl_CGRect value;
		} rect;
		struct {
			dl_CGRect inRect;
		} ellipse;
		struct {
			dl_CGRect rect;
			dl_CGFloat cornerWidth, cornerHeight;
		} rounded;
		struct {
			const dl_CGAffineTransform *check;   // used to quickly check that all segments are using the same transform
			//std::vector<PathSegment> segments; // moved out of union because issues
		} segmented;
	};

	SubPath(SubPathTag tag) {
		this->tag = tag;
	}

	bool isRect() {
		return tag == Tag_Rect;
	}

	dl_CGRect getRect() {
		return rect.value;
	}

	void concatTransform(const dl_CGAffineTransform *t) {
		if (!t) return;
		if (hasTransform) {
			transform = dl_CGAffineTransformConcat(transform, *t); // verify order etc
			// do we need to set the 'check' pointer if this is a segmented path?
		}
		else {
			hasTransform = true;
			transform = *t;
		}
	}

	static SubPath createRect(const dl_CGRect &r, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_Rect);
		sp.rect.value = r;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		return std::move(sp);
	}
	static SubPath createEllipse(const dl_CGRect &r, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_Ellipse);
		sp.ellipse.inRect = r;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		return std::move(sp);
	}
	static SubPath createRoundedRect(const dl_CGRect &r, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_RoundedRect);
		sp.rounded.rect = r;
		sp.rounded.cornerWidth = cornerWidth;
		sp.rounded.cornerHeight = cornerHeight;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		return std::move(sp);
	}
	static SubPath createEmptyPath() {
		SubPath sp(Tag_Segmented);
		sp.segmented.check = nullptr;
		// no transform yet, that will be initialized/verified with each segment add
		return std::move(sp);
	}

	void validateTransform(const dl_CGAffineTransform *m) {
		if (m != segmented.check) {
			// either it's the first provided one, or a new one mid-subpath which we don't yet support
			if (segmented.check == nullptr && segments.size() == 0) {
				// first element, first provided matrix - OK
				segmented.check = m; // will be used to verify subsequent segments - they all must match, because subpath/geom transformed as a unit
				transform = *m; // copy
				hasTransform = true;
			}
			else {
				throw cf::Exception("CGPath SubPath validation: provided transform didn't match existing for current subpath");
			}
		}
	}

	inline void moveToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkMoveToPoint(x, y)));
	}
	
	inline void addLineToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkLineToPoint(x, y)));
	}

	inline void addLines(const dl_CGAffineTransform *m, const dl_CGPoint *points, size_t count) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		//
		if (count < 1) return;
		segments.push_back(std::move(PathSegment::mkMoveToPoint(points->x, points->y)));
		for (size_t i = 1; i < count; i++) {
			auto p = &points[i];
			segments.push_back(std::move(PathSegment::mkLineToPoint(p->x, p->y)));
		}
	}

	inline void addArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkArc(x, y, radius, startAngle, endAngle, clockwise)));
	}

	inline void addRelativeArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkRelativeArc(x, y, radius, startAngle, delta)));
	}

	inline void addArcToPoint(const dl_CGAffineTransform *m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkArcToPoint(x1, y1, x2, y2, radius)));
	}

	inline void addCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkCurveToPoint(cp1x, cp1y, cp2x, cp2y, x, y)));
	}

	inline void addQuadCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkQuadCurveToPoint(cpx, cpy, x, y)));
	}

	inline void close() {
		if (tag != Tag_Segmented) return;
		segments.push_back(std::move(PathSegment::mkClosure()));
	}
};

class CGPath; typedef CGPath* CGPathRef;
class CGPath : public cf::Object {
protected:
	std::vector<SubPath> subPaths;

	SubPath *currentSub() {
		return &subPaths[subPaths.size() - 1];
	}

public:
	const char *getTypeName() const override { return "CGPath"; }

	CGPath(SubPath sub) {
		subPaths.push_back(sub);
	}

	CGPath(const CGPath &other) {
		subPaths = other.subPaths;
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

	// this is inverted, eventually it's the context that will have a method to add paths
	// (we'll append to an existing sub/path)
	void addToContext(dl_CGContextRef c);

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
	CGMutablePath()
		: CGPath(SubPath::createEmptyPath()), open(true)
	{}

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
		mutableSub()->addRelativeArc(matrix, x, y, radius, startAngle, delta);
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
		subPaths.push_back(std::move(SubPath::createRect(rect, m)));
	}
	void addRects(const dl_CGAffineTransform *m, const dl_CGRect *rects, size_t count) {
		open = false;
		for (size_t i = 0; i < count; i++) {
			subPaths.push_back(std::move(SubPath::createRect(rects[i], m)));
		}
	}
	void addRoundedRect(const dl_CGAffineTransform *transform, dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight) {
		open = false;
		subPaths.push_back(std::move(SubPath::createRoundedRect(rect, cornerWidth, cornerHeight, transform)));
	}
	void addEllipseInRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
		open = false;
		subPaths.push_back(std::move(SubPath::createEllipse(rect, m)));
	}
	void addPath(const dl_CGAffineTransform *m, dl_CGPathRef path2) {
		open = false;
		auto sourceSubs = ((CGPathRef)path2)->getSubPaths();
		for (auto i = sourceSubs.begin(); i != sourceSubs.end(); i++) {
			SubPath sp = (*i);
			sp.concatTransform(m); // handles null transforms internally
			subPaths.push_back(std::move(sp));
		}
	}
	void closeSubpath() {
		mutableSub()->close();
		open = false; // will need to push a new segmented subpath to add further segments (mutableSub() takes care of it)
	}

	RETAIN_AND_AUTORELEASE(CGMutablePath)
};
