#ifndef __PAGE04_HPP__
#define __PAGE04_HPP__

#include "common.h"

class CPage04 : public PageCommon {
public:
	CPage04(WindowFuncs *parent) 
		: PageCommon(parent) {}
	void render(dl_CGContextRef context, int width, int height) override {
		dl_CGPoint start;
		dl_CGPoint end;

		auto grad1 = getGradient(0.3, 0.2, 1, 1, 1, 0.3, 0, 1);
		start = { 100, 100 };
		end = { (dl_CGFloat)width - 100, (dl_CGFloat)height - 100 };
		dl_CGContextDrawLinearGradient(context, grad1, start, end, dl_kCGGradientDrawsBeforeStartLocation | dl_kCGGradientDrawsAfterEndLocation);
		dl_CGGradientRelease(grad1);

		auto grad2 = getGradient(0, 0, 0, 0.5, 1, 1, 1, 0.5);
		start = { 100, (dl_CGFloat)height - 100 };
		end = { (dl_CGFloat)width - 100, 100 };
		dl_CGContextDrawLinearGradient(context, grad2, start, end, 0);
		dl_CGGradientRelease(grad2);

		auto grad3 = getGradient(0, 1, 1, 1, 0, 0, 0, 1);
		start = { (dl_CGFloat)width / 2 - 50, 0 };
		end = { (dl_CGFloat)width / 2 + 50, 0 };
		dl_CGContextDrawLinearGradient(context, grad3, start, end, 0);
		dl_CGGradientRelease(grad3);

		auto grad4 = getGradient(1, 0, 1, 0.5, 0, 0, 0, 0.5);
		start = { 0, (dl_CGFloat)height / 2 - 50 };
		end = { 0, (dl_CGFloat)height / 2 + 50 };
		dl_CGContextDrawLinearGradient(context, grad4, start, end, 0);
		dl_CGGradientRelease(grad4);
	}
};


#endif
