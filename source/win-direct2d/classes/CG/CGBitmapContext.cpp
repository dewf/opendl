#include "CGBitmapContext.h"

CGBitmapContext::CGBitmapContext(ID2D1RenderTarget * wicTarget, IWICBitmap * wicBitmap)
	:CGContext(wicTarget), wicBitmap(wicBitmap)
{
	// don't AddRef anything, we own it
	// (note that the parent CGBitmap class doesn't release anything in its destructor, since it doesn't own anything)

	target->BeginDraw(); // in drawing mode by default, unless we're making a copy
}

CGBitmapContext::~CGBitmapContext()
{
	target->EndDraw();
	//
	target->Release();
	wicBitmap->Release();

	// release the target-dependent resources
	::deleteCacheForTarget(target);
}

CGImageRef CGBitmapContext::createImage()
{
	target->EndDraw(); // temporarily stop drawing
	//target->Flush(NULL, NULL);

	IWICBitmap *outBitmap;
	HR(wicFactory->CreateBitmapFromSource(wicBitmap, WICBitmapCacheOnLoad, &outBitmap)); // don't use WICBitmapCacheOnDemand, because (I'm guessing) after we re-enable BeginDraw, the image is locked and the copy can't demand access to it

	target->BeginDraw(); // resume drawing
	return new CGImage(outBitmap); // ownership passes to CGImage
}

void * CGBitmapContext::lockData()
{
	target->EndDraw(); // stop drawing until unlock

	UINT width, height;
	wicBitmap->GetSize(&width, &height);
	WICRect rect = { 0, 0, (INT)width, (INT)height };
	HR(wicBitmap->Lock(&rect, WICBitmapLockRead | WICBitmapLockWrite, &lock));

	UINT bufferSize;
	// buffer is member field
	HR(lock->GetDataPointer(&bufferSize, &buffer));

	return buffer;
}

void CGBitmapContext::unlockData()
{
	if (lock) {
		// swizzle RGB ... not sure why this is necessary, seems backwards given the pixel formats specified in dl_CGBitmapContextCreate
		UINT width, height;
		UINT stride;
		lock->GetSize(&width, &height);
		lock->GetStride(&stride);
		auto data = (UINT *)buffer;
		for (UINT i = 0; i < height; i++) {
			for (UINT j = 0; j < width; j++) {
				auto x = data[j];
				auto r = x & 0xFF;
				auto b = (x >> 16) & 0xFF;
				auto a_g = x & 0xFF00FF00; // maintain position/values of alpha and green
				data[j] = a_g | (r << 16) | b;
			}
			data += stride / sizeof(UINT);
		}

		lock->Release();
		lock = nullptr;

		target->BeginDraw(); // resume drawing
	}
}
