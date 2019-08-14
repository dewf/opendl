#pragma once

#include "../../../common/classes/CF/CFTypes.h"
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
	CGPath(const CGPath &other) {
		items = other.items;
	}
	~CGPath() {}

	static CGPathRef createWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
		if (transform) {
			throw cf::Exception("transforms not currently supported in CGPath constructor");
		}
		// otherwise we'd transform the points before adding them
		auto ret = new CGPath();
		PathItem item;
		item.tag = PathItem::Rect;
		item.rect.value = rect;
		ret->items.push_back(item);
		return ret;
	}

	static CGPathRef createWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
		if (transform) {
			throw cf::Exception("transforms not currently supported in CGPath constructor (createWithEllipseInRect)");
		}
		// otherwise transform ...
		auto ret = new CGPath();
		PathItem item;
		item.tag = PathItem::Ellipse;
		item.ellipse.inRect = rect;
		ret->items.push_back(item);
		return ret;
	}

	static CGPathRef createWithRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform)
	{
		if (transform) {
			throw cf::Exception("transforms not currently supported in CGPath constructor (createWithEllipseInRect)");
		}
		// otherwise transform
		auto ret = new CGPath();
		PathItem item;
		item.tag = PathItem::RoundedRect;
		item.rounded.rect = rect;
		item.rounded.cornerWidth = cornerWidth;
		item.rounded.cornerHeight = cornerHeight;
		ret->items.push_back(item);
		return ret;
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
	void addRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
		if (m) {
			throw cf::Exception("transforms not currently supported in CGMutablePath constructor");
		}
		PathItem item;
		item.tag = PathItem::Rect;
		item.rect.value = rect;

		items.push_back(item);
	}

	virtual bool operator <(const Object &b) const {
		throw cf::Exception("operator < not defined for CGMutablePath");
	}
	virtual CGPathRef copy() {
		// make an immutable copy
		return new CGPath(*this);
	}

	RETAIN_AND_AUTORELEASE(CGMutablePath)
};
