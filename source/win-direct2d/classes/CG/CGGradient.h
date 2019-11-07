#pragma once

//#include <d2d1.h>
//#include <d2d1helper.h>

#include "../../../opendl.h"
#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "CGColorSpace.h"

class CGGradient; typedef CGGradient* CGGradientRef;
class CGGradient : public cf::Object {
protected:
	D2D1_GRADIENT_STOP * stops = NULL; // allocated array, only thing we can really cache about these (even creating a gradstopcollection requires an ID2D1 target, not available in the dl_CGGradientCreateWithColorComponents call)
	size_t stopCount = 0;
	CGColorSpaceRef space;
public:
	const char *getTypeName() const override { return "CGGradient"; }

	CGGradient(CGColorSpaceRef space, const dl_CGFloat components[], const dl_CGFloat locations[], size_t count) {
		this->space = (CGColorSpaceRef)space->retain();
		stopCount = count;
		stops = new D2D1_GRADIENT_STOP[count];
		for (size_t i = 0; i < count; i++) {
			stops[i].position = (FLOAT)locations[i];
			const dl_CGFloat *compBase = &components[i * 4];
			stops[i].color = D2D1::ColorF((FLOAT)compBase[0], (FLOAT)compBase[1], (FLOAT)compBase[2], (FLOAT)compBase[3]);
		}
	}

	~CGGradient() override {
		space->release();
		if (stops) {
			delete stops;
			stops = nullptr;
			stopCount = 0;
		}
	}

	virtual std::string toString() const {
		char buffer[1024];
		snprintf(buffer, 1024, "CGGradient@%p: %zd stops, (space [%s]) [%s]", this, stopCount, space->toString().c_str(), Object::toString().c_str());
		return std::string(buffer);
	}

	const D2D1_GRADIENT_STOP *getStops() { return stops; }
	const size_t getStopCount() { return stopCount; }

	static CGGradientRef createWithColorComponents(CGColorSpaceRef space, const dl_CGFloat components[], const dl_CGFloat locations[], size_t count)
	{
		return new CGGradient(space, components, locations, count);
	}

	virtual CGGradientRef copy() {
		// immutable so just retain
		return (CGGradientRef)retain();
	}

	RETAIN_AND_AUTORELEASE(CGGradient)
};
