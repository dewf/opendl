#pragma once

#include "CGContext.h"
#include "CGImage.h"

#include "../../private_defs.h"
#include "../../COMStuff.h"

class CGBitmapContext; typedef CGBitmapContext* CGBitmapContextRef;
class CGBitmapContext : public CGContext {
	ID2D1RenderTarget *wicTarget = nullptr;
	IWICBitmap *wicBitmap = nullptr;
	IWICBitmapLock *lock = nullptr;
	WICInProcPointer buffer = nullptr;
public:
	CGBitmapContext(ID2D1RenderTarget *wicTarget, IWICBitmap *wicBitmap);
	virtual ~CGBitmapContext() override; 

	CGImageRef createImage();

	void *lockData();
	void unlockData();

	RETAIN_AND_AUTORELEASE(CGBitmapContext)
};