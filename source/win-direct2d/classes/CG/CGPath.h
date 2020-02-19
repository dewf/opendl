#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include <vector>

struct PathItem {
	enum {
		Rect,
		Ellipse,
		RoundedRect
	} tag;
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
	};
};

class CGPath; typedef CGPath* CGPathRef;
class CGPath : public cf::Object {
protected:
	std::vector<PathItem> items;
public:
	const char *getTypeName() const override { return "CGPath"; }

	CGPath() {}
	CGPath(PathItem &item) {
		items.push_back(item);
	}
	CGPath(const CGPath &other) {
		items = other.items;
	}
	~CGPath() {}

	static CGPathRef createWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
		if (transform) {
			throw cf::Exception("transforms not currently supported in CGPath constructor");
		}
		// otherwise we'd transform the points before adding them
		PathItem item;
		item.tag = PathItem::Rect;
		item.rect.value = rect;
		return new CGPath(item);
	}

	static CGPathRef createWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
		if (transform) {
			throw cf::Exception("transforms not currently supported in CGPath constructor (createWithEllipseInRect)");
		}
		// otherwise transform ...
		PathItem item;
		item.tag = PathItem::Ellipse;
		item.ellipse.inRect = rect;
		return new CGPath(item);
	}

	static CGPathRef createWithRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform)
	{
		if (transform) {
			throw cf::Exception("transforms not currently supported in CGPath constructor (createWithEllipseInRect)");
		}
		// otherwise transform
		PathItem item;
		item.tag = PathItem::RoundedRect;
		item.rounded.rect = rect;
		item.rounded.cornerWidth = cornerWidth;
		item.rounded.cornerHeight = cornerHeight;
		return new CGPath(item);
	}

	//std::unique_ptr<CGPath, ObjReleaser> autoRelease() {
	//	// automatically calls ->release() when it goes out of scope :)
	//	return std::unique_ptr<CGPath, ObjReleaser>(this);
	//}

	// this is inverted, eventually it's the context that will have a method to add paths
	// (we'll append to an existing sub/path)
	void addToContext(dl_CGContextRef c);

	bool isRect() {
		return (items.size() == 1 && items[0].tag == PathItem::Rect);
	}

	dl_CGRect getRect() {
		if (isRect()) {
			return items[0].rect.value;
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
public:
	CGMutablePath() : CGPath() {}
	~CGMutablePath() {}

	virtual bool operator <(const Object &b) const {
		throw cf::Exception("operator < not defined for CGMutablePath");
	}
	virtual CGPathRef copy() {
		// make an immutable copy
		return new CGPath(*this);
	}

	void moveToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y);
	void addArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise);
	void addRelativeArc(const dl_CGAffineTransform *matrix, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta);
	void addArcToPoint(const dl_CGAffineTransform *m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius);
	void addCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y);
	void addLines(const dl_CGAffineTransform *m, const dl_CGPoint *points, size_t count);
	void addLineToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y);
	void addPath(const dl_CGAffineTransform *m, dl_CGPathRef path2);
	void addQuadCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y);
	void addRect(const dl_CGAffineTransform *m, dl_CGRect rect);
	void addRects(const dl_CGAffineTransform *m, const dl_CGRect *rects, size_t count);
	void addRoundedRect(const dl_CGAffineTransform *transform, dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight);
	void addEllipseInRect(const dl_CGAffineTransform *m, dl_CGRect rect);
	void closeSubpath();

	RETAIN_AND_AUTORELEASE(CGMutablePath)
};
