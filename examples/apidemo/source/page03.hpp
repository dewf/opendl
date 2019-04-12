#ifndef __PAGE03_HPP__
#define __PAGE03_HPP__

#include "common.h"

class CPage03 : public PageCommon {
public:
	CPage03(WindowFuncs *parent) 
		: PageCommon(parent) {}
	void render(dl_CGContextRef c, int width, int height) override {
		dl_CGContextSetRGBFillColor(c, 0.5, 0.5, 0.5, 1);
		dl_CGContextFillRect(c, { {0, 0}, {(float)width, (float)height} });
		dl_CGContextScaleCTM(c, 3.0, 3.0);

		dl_CGContextSetTextMatrix(c, dl_CGAffineTransformIdentity);

		auto p = dl_CGPointMake(20, 20);
		dl_CGRect r;
		r.origin.x = p.x;
		r.origin.y = p.y;

		auto str2 = dl_CFSTR("Quartz♪❦♛あぎ");
		auto attrs2 = dl_CFDictionaryCreateMutable(10);

		/**** draw with default attributes ****/
		pointAt(c, p);
		textLine(c, p.x, p.y, str2, attrs2);

		/**** draw with specific font and color ****/
		p.y += 30;
		pointAt(c, p);

		// larger font + blue color
		auto largeFont = dl_CTFontCreateWithName(dl_CFSTR(TIMES_FONT_NAME), 40.0, NULL);
		auto blueColor = dl_CGColorCreateGenericRGB(0, 0.3, 1, 1);

		dl_CFDictionarySetValue(attrs2, dl_kCTFontAttributeName, largeFont); //TimesNewRomanPSMT
		dl_CFDictionarySetValue(attrs2, dl_kCTForegroundColorAttributeName, blueColor);
		textLine(c, p.x, p.y, str2, attrs2);

		/**** line3: draw stroked text ****/
		p.y += 60;
		pointAt(c, p);

		auto strokeWidth = dl_CFNumberWithFloat(1);
		dl_CFDictionarySetValue(attrs2, dl_kCTStrokeWidthAttributeName, strokeWidth);
		textLine(c, p.x, p.y, str2, attrs2);

		/**** line 4: fill and stroke ****/
		p.y += 60;
		pointAt(c, p);

		// negative stroke = stroke and fill
		auto strokeWidth2 = dl_CFNumberWithFloat(-1);
		auto black = dl_CGColorCreateGenericRGB(0, 0, 0, 1);
		dl_CFDictionarySetValue(attrs2, dl_kCTStrokeWidthAttributeName, strokeWidth2);
		dl_CFDictionarySetValue(attrs2, dl_kCTStrokeColorAttributeName, black);
		textLine(c, p.x, p.y, str2, attrs2);

		/**** line 5: draw at baseline ****/
		p.y += 100;
		pointAt(c, p);

		textLine(c, p.x, p.y, str2, attrs2, false, true);

		// release
		dl_CFRelease(attrs2);
		dl_CFRelease(black);
		dl_CFRelease(strokeWidth2);
		dl_CFRelease(strokeWidth);
		dl_CFRelease(blueColor);
		dl_CFRelease(largeFont);
	}
};

#endif
