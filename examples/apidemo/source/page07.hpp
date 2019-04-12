#ifndef __PAGE07_HPP__
#define __PAGE07_HPP__

#include "common.h"

void clipPre(dl_CGContextRef c, dl_CGTextDrawingMode mode, dl_CGRect bounds, dl_CGFloat xpos, dl_CGFloat ypos) {
	if (mode == dl_kCGTextClip ||
		mode == dl_kCGTextFillClip ||
		mode == dl_kCGTextStrokeClip ||
		mode == dl_kCGTextFillStrokeClip)
	{
		dl_CGContextSaveGState(c);
		//
		auto x = dl_CGRectMake(xpos, ypos, bounds.size.width, bounds.size.height);
		dl_CGContextStrokeRect(c, x);
	}
}

void clipPost(dl_CGContextRef c, dl_CGTextDrawingMode mode, dl_CGRect bounds, dl_CGFloat xpos, dl_CGFloat ypos) {
	static auto grad = getGradient(1, 0, 1, 1, 0, 1, 1, 1);
	if (mode == dl_kCGTextClip ||
		mode == dl_kCGTextFillClip ||
		mode == dl_kCGTextStrokeClip ||
		mode == dl_kCGTextFillStrokeClip)
	{
		auto start = dl_CGPointMake(xpos, ypos);
		auto end = dl_CGPointMake(xpos + bounds.size.width, ypos + bounds.size.height);
		dl_CGContextDrawLinearGradient(c, grad, start, end, 0);
		//
		dl_CGContextRestoreGState(c);
	}
}

void strokeTesting(dl_CGContextRef c, dl_CFMutableAttributedStringRef attrString, dl_CTFontRef font, dl_CFBooleanRef useForegroundColor, dl_CGFloat xpos) {
	auto strLen = dl_CFAttributedStringGetLength(attrString);

	// dict
	dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(0, strLen), dl_kCTFontAttributeName, font);
	dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(0, strLen), dl_kCTForegroundColorFromContextAttributeName, useForegroundColor);
	auto line0 = dl_CTLineCreateWithAttributedString(attrString);

	// variations
	auto as1 = dl_CFAttributedStringCreateMutableCopy(0, attrString);
	auto n2 = dl_CFNumberWithFloat(2);
	dl_CFAttributedStringSetAttribute(as1, dl_CFRangeMake(0, strLen), dl_kCTStrokeWidthAttributeName, n2);
	auto line1 = dl_CTLineCreateWithAttributedString(as1);

	auto as2 = dl_CFAttributedStringCreateMutableCopy(0, attrString);
	auto neg2 = dl_CFNumberWithFloat(-2);
	dl_CFAttributedStringSetAttribute(as2, dl_CFRangeMake(0, strLen), dl_kCTStrokeWidthAttributeName, neg2);
	auto line2 = dl_CTLineCreateWithAttributedString(as2);

	auto green = dl_CGColorCreateGenericRGB(0, 1, 0, 1);
	auto as3 = dl_CFAttributedStringCreateMutableCopy(0, attrString);
	dl_CFAttributedStringSetAttribute(as3, dl_CFRangeMake(0, strLen), dl_kCTStrokeWidthAttributeName, n2);
	dl_CFAttributedStringSetAttribute(as3, dl_CFRangeMake(0, strLen), dl_kCTStrokeColorAttributeName, green);
	auto line3 = dl_CTLineCreateWithAttributedString(as3);

	dl_CGContextSetRGBFillColor(c, 0, 0.8, 1, 1);
	dl_CGContextSetRGBStrokeColor(c, 1, 1, 0, 1);
	dl_CGContextSetLineWidth(c, 1.5);

	dl_CGFloat ascent, descent, leading, lineWidth;
	lineWidth = dl_CTLineGetTypographicBounds(line0, &ascent, &descent, &leading);

	auto rect = dl_CTLineGetBoundsWithOptions(line0, dl_kCTLineBoundsUseOpticalBounds);

	dl_CGFloat ypos = 0;

	dl_CGTextDrawingMode modeList[] = { dl_kCGTextFill, dl_kCGTextStroke, dl_kCGTextFillStroke, dl_kCGTextStrokeClip };
	size_t num_modes = sizeof(modeList) / sizeof(dl_kCGTextFill);

	for (size_t i = 0; i < num_modes; i++) {
		auto mode = modeList[i];
		dl_CGContextSetTextDrawingMode(c, mode);

		clipPre(c, mode, rect, xpos, ypos);
		dl_CGContextSetTextPosition(c, xpos, ypos + ascent);
		dl_CTLineDraw(line0, c);
		clipPost(c, mode, rect, xpos, ypos);
		ypos += 60;

		//clipPre(c, mode, rect, xpos, ypos);
		dl_CGContextSetTextPosition(c, xpos + 20, ypos + ascent);
		dl_CTLineDraw(line1, c);
		//clipPost(c, mode, rect, xpos, ypos);
		ypos += 60;

		//clipPre(c, mode);
		dl_CGContextSetTextPosition(c, xpos + 40, ypos + ascent);
		dl_CTLineDraw(line2, c);
		//clipPost(c, mode, rect);
		ypos += 60;

		//clipPre(c, mode);
		dl_CGContextSetTextPosition(c, xpos + 60, ypos + ascent);
		dl_CTLineDraw(line3, c);
		//clipPost(c, mode, rect);
		ypos += 60;
	}

	dl_CFRelease(n2);
	dl_CFRelease(neg2);
	dl_CFRelease(line3);
	dl_CFRelease(as3);
	dl_CFRelease(green);
	dl_CFRelease(line2);
	dl_CFRelease(as2);
	dl_CFRelease(line1);
	dl_CFRelease(as1);
	dl_CFRelease(line0);
}

class CPage07 : public PageCommon {
public:
	CPage07(WindowFuncs *parent) 
		: PageCommon(parent) {}
	void render(dl_CGContextRef c, int width, int height) override {
		dl_CGContextSetRGBFillColor(c, 0.5, 0.5, 0.5, 1);
		dl_CGContextFillRect(c, dl_CGRectMake(0, 0, width, height));

		dl_CGContextSetTextMatrix(c, dl_CGAffineTransformIdentity);

		auto font = dl_CTFontCreateWithName(dl_CFSTR(TIMES_FONT_NAME), 70, NULL);
		auto attrString = dl_CFAttributedStringCreateMutable(0);

		dl_CFAttributedStringReplaceString(attrString, dl_CFRangeMake(0, 0), dl_CFSTR("Testing 123!"));

		strokeTesting(c, attrString, font, dl_kCFBooleanFalse, 0);
		strokeTesting(c, attrString, font, dl_kCFBooleanTrue, 400);

		dl_CFRelease(attrString);
		dl_CFRelease(font);
	}
};

#endif
