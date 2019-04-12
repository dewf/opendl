#pragma once

#include "CGContext.h"
#include "CGImage.h"

#include "../../private_defs.h"
#include "../../COMStuff.h"

class CGBitmapContext; typedef CGBitmapContext* CGBitmapContextRef;
class CGBitmapContext : public CGContext {
	ID2D1RenderTarget *wicTarget;
	IWICBitmap *wicBitmap;
public:
	CGBitmapContext(ID2D1RenderTarget *wicTarget, IWICBitmap *wicBitmap)
		:CGContext(wicTarget), wicBitmap(wicBitmap)
	{
		// don't AddRef anything, we own it
		// (note that the parent CGBitmap class doesn't release anything in its destructor, since it doesn't own anything)

		target->BeginDraw(); // in drawing mode by default, unless we're making a copy
	}
	virtual ~CGBitmapContext() override {
		target->EndDraw();
		//
		target->Release();
		wicBitmap->Release();

		// release the target-dependent resources
		::deleteCacheForTarget(target);
	}

	CGImageRef createImage() {
		target->EndDraw(); // temporarily stop drawing
		//target->Flush(NULL, NULL);

		IWICBitmap *outBitmap;
		HR(wicFactory->CreateBitmapFromSource(wicBitmap, WICBitmapCacheOnLoad, &outBitmap)); // don't use WICBitmapCacheOnDemand, because (I'm guessing) after we re-enable BeginDraw, the image is locked and the copy can't demand access to it

		target->BeginDraw(); // resume drawing
		return new CGImage(outBitmap); // ownership passes to CGImage
	}

	RETAIN_AND_AUTORELEASE(CGBitmapContext)
};