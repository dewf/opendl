#pragma once

#include "../../../opendl.h"
#include "../../../common/classes/CF/CFTypes.h"
#include "../../COMStuff.h"

#include <wincodec.h>

class CGImage; typedef CGImage* CGImageRef;
class CGImage : public cf::Object {
private:
	IWICBitmap *wicBitmap;
public:
	const char *getTypeName() const override { return "CGImage"; }

	CGImage(IWICBitmap *bitmap)
		:wicBitmap(bitmap)
	{
		// no AddRef since we own it
	}
	~CGImage() override
	{
		wicBitmap->Release();
	}

	inline void getSize(UINT *width, UINT *height) {
		wicBitmap->GetSize(width, height);
	}

	ID2D1Bitmap *getBitmap(ID2D1RenderTarget *target) {
		ID2D1Bitmap *outBitmap;
		HR(target->CreateBitmapFromWicBitmap(wicBitmap, &outBitmap));
		return outBitmap;
	}

	ID2D1Bitmap *getAlphaBitmapFromLuminance(ID2D1RenderTarget *target);

	virtual CGImageRef copy() override {
		throw cf::Exception("copy() not yet implemented for CGImage");
	}

	RETAIN_AND_AUTORELEASE(CGImage)
};
