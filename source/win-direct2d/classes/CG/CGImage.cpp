#include "CGImage.h"

#include "../../../common/util.h" // for luminanceToAlpha

ID2D1Bitmap *CGImage::getAlphaBitmapFromLuminance(ID2D1RenderTarget *target) {
	// quartz uses grayscale images (with no alpha) for masking
	// this is our approximation of that ...
	UINT width, height;
	getSize(&width, &height);

	WICRect rect{ 0, 0, (INT)width, (INT)height };
	IWICBitmapLock *lock;
	wicBitmap->Lock(&rect, WICBitmapLockRead | WICBitmapLockWrite, &lock);

	UINT bufferSize = 0;
	UINT stride = 0;
	BYTE *row = NULL;
	HR(lock->GetStride(&stride));
	HR(lock->GetDataPointer(&bufferSize, &row));
	for (UINT i = 0; i < height; i++) {
		auto pixel = ((UINT32 *)row);
		for (UINT j = 0; j < width; j++) {
			luminanceToAlpha(*pixel++);
		}
		row += stride;
	}
	lock->Release();
	// if we were using a Device Context (inherits from Render Target), then we could use D2D effects to perform this for us
	// look into that at a later time - how much would it ruin backward compatibility?
	// apparently Win7 requires the "Platform Update for Windows 7" to use a device context with Direct2D (vs. the HWND Render Target)

	// ... and then return the D2D bitmap
	ID2D1Bitmap *outBitmap;
	HR(target->CreateBitmapFromWicBitmap(wicBitmap, &outBitmap));
	return outBitmap;
}
