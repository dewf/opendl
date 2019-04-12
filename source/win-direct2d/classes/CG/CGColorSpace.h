#pragma once

#include "../../../common/classes/CF/CFTypes.h"

class CGColorSpace; typedef CGColorSpace* CGColorSpaceRef;
class CGColorSpace : public cf::Object {
	// currently this does/means nothing,
	// need to figure out if there is even a way to do this with Direct2D
	// seems like you have to create the backbuffers with a color model (like SRGB)
protected:
	cf::StringRef colorSpaceName;
public:
	static const cf::StringRef kCGColorSpace__InternalDeviceGray;

	const char *getTypeName() const override { return "CGColorSpace"; }

	CGColorSpace(cf::StringRef colorSpaceName)
		:colorSpaceName(colorSpaceName->copy()) {}

	~CGColorSpace() {
		colorSpaceName->release();
	}

	const cf::StringRef getColorSpaceName() const {
		return colorSpaceName;
	}

	virtual CGColorSpaceRef copy() {
		// immutable so just retain
		return (CGColorSpaceRef)retain();
	}

	virtual std::string toString() const {
		return sprintfToStdString("CGColorSpace@%p: {%s} [%s]", this, colorSpaceName->getStdString().c_str(), Object::toString().c_str());
	}

	RETAIN_AND_AUTORELEASE(CGColorSpace)
};
