#pragma once

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"
#include <vector>

class CGPath; typedef CGPath* CGPathRef;
class CGPath : public cf::Object {
protected:
	std::vector<dl_CGRect> rects;
public:
	const char *getTypeName() const override { return "CGPath"; }

	CGPath() {}
	CGPath(dl_CGRect rect, const dl_CGAffineTransform *transform) {
		if (transform) {
			throw cf::Exception("transforms not currently supported in CGPath constructor");
		}
		// otherwise we'd transform the points before adding them
		rects.push_back(rect);
	}
	CGPath(const CGPath &other) {
		rects = other.rects;
	}
	~CGPath() {}

	//std::unique_ptr<CGPath, ObjReleaser> autoRelease() {
	//	// automatically calls ->release() when it goes out of scope :)
	//	return std::unique_ptr<CGPath, ObjReleaser>(this);
	//}

	// this is inverted, eventually it's the context that will have a method to add paths
	// (we'll append to an existing sub/path)
	void addToContext(dl_CGContextRef c) {
		for (auto i = rects.begin(); i != rects.end(); i++) {
			dl_CGContextAddRect(c, *i);
		}
	}

	bool isRect() {
		return (rects.size() == 1);
	}
	dl_CGRect getRect() {
		if (isRect()) {
			return rects[0];
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
		rects.push_back(rect);
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
