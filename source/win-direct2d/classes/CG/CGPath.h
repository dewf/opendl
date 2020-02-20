#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include <vector>

#include <utility>

struct PathSegment {
	enum TagEnum {
		Tag_StartPoint,
		Tag_LineToPoint,
		Tag_Rect,
		Tag_Arc,
		Tag_ArcToPoint, // actually a line ending with an arc
		Tag_Ellipse,
		Tag_RoundedRect,
		Tag_Closure
	} tag;
	union {
		struct {
			dl_CGFloat x, y;
		} point; // StartPoint, LineToPoint
		dl_CGRect rect;
		struct {
			dl_CGFloat x, y, radius, startAngle, endAngle;
			int clockwise;
		} arc;
		// TODO: should probably replace the above point/rect/arc with D2D structs ...
		D2D1_ELLIPSE ellipse;
		D2D1_ROUNDED_RECT roundedRect;
		struct {
			dl_CGFloat x1, y1, x2, y2, radius;
		} arcToPoint;
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

	std::vector<PathSegment> segments;  // belongs in the path struct but that causes C++ issues so ... here instead

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
			const dl_CGAffineTransform *check;  // used to quickly check that all segments are using the same transform
		} path;
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

	static SubPath createRect(dl_CGRect &r, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_Rect);
		sp.rect.value = r;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		return sp;
	}
	static SubPath createEllipse(dl_CGRect &r, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_Ellipse);
		sp.ellipse.inRect = r;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		return sp;
	}
	static SubPath createRoundedRect(dl_CGRect &r, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_RoundedRect);
		sp.rounded.rect = r;
		sp.rounded.cornerWidth = cornerWidth;
		sp.rounded.cornerHeight = cornerHeight;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		return sp;
	}
	static SubPath createEmptyPath() {
		SubPath sp(Tag_Segmented);
		sp.path.check = nullptr;
		// no transform yet, that will be initialized/verified with each segment add
		return sp;
	}

	void validateTransform(const dl_CGAffineTransform *m) {
		if (m != path.check) {
			// either it's the first provided one, or a new one mid-subpath which we don't yet support
			if (path.check == nullptr && segments.size() == 0) {
				// first element, first provided matrix - OK
				path.check = m; // will be used to verify subsequent segments - they all must match, because subpath/geom transformed as a unit
				transform = *m; // copy
				hasTransform = true;
			}
			else {
				throw cf::Exception("CGPath SubPath validation: provided transform didn't match existing for current subpath");
			}
		}
	}

	inline void moveToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
		if (!tag == Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkMoveToPoint(x, y)));
	}

	inline void addArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise) {
		if (!tag == Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(std::move(PathSegment::mkArc(x, y, radius, startAngle, endAngle, clockwise)));
	}

	inline void close() {
		if (!tag == Tag_Segmented) return;
		PathSegment seg(PathSegment::Tag_Closure);
		segments.push_back(seg);
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

	RETAIN_AND_AUTORELEASE(CGPath)
};


class CGMutablePath; typedef CGMutablePath* CGMutablePathRef;
class CGMutablePath : public CGPath {
private:
	bool open = false; // is there a segmented path on the stack, ready to go?
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
		currentSub()->moveToPoint(m, x, y);
	}
	void addArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise) {
		currentSub()->addArc(m, x, y, radius, startAngle, endAngle, clockwise);
	}
	void addRelativeArc(const dl_CGAffineTransform *matrix, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta) {
	}
	void addArcToPoint(const dl_CGAffineTransform *m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius) {
	}
	void addCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y) {
	}
	void addLines(const dl_CGAffineTransform *m, const dl_CGPoint *points, size_t count) {
	}
	void addLineToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
	}
	void addPath(const dl_CGAffineTransform *m, dl_CGPathRef path2) {
	}
	void addQuadCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y) {
	}
	void addRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
	}
	void addRects(const dl_CGAffineTransform *m, const dl_CGRect *rects, size_t count) {
	}
	void addRoundedRect(const dl_CGAffineTransform *transform, dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight) {
	}
	void addEllipseInRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
	}
	void closeSubpath() {
		currentSub()->close();
		open = false; // will need to push a new segmented subpath to add further segments (getCurrentSegmented() takes care of it)
	}

	RETAIN_AND_AUTORELEASE(CGMutablePath)
};
