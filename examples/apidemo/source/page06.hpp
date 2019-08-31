#ifndef __PAGE06_HPP__
#define __PAGE06_HPP__

#include "common.h"

void drawTextInShape(dl_CGContextRef context, int viewHeight, dl_CGPathRef path, dl_CFStringRef text, dl_CGColorRef color, dl_CTFontRef font, dl_CTTextAlignment *alignment = NULL)
{
	auto attrString = dl_CFAttributedStringCreateMutable(0); // 0 = amt of storage to reserve, no hint
	dl_CFAttributedStringReplaceString(attrString, dl_CFRangeMake(0, 0), text);
	auto strLen = dl_CFAttributedStringGetLength(attrString);

	dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(0, strLen), dl_kCTForegroundColorAttributeName, color);
	dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(0, strLen), dl_kCTFontAttributeName, font);
	if (alignment) {
		dl_CTParagraphStyleSetting pss;
		pss.spec = dl_kCTParagraphStyleSpecifierAlignment;
		auto value = *alignment;
		pss.value = &value;
		pss.valueSize = sizeof(value);
		auto style = dl_CTParagraphStyleCreate(&pss, 1);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(0, strLen), dl_kCTParagraphStyleAttributeName, style);
		dl_CFRelease(style);
	}

	// framesetter
	auto framesetter = dl_CTFramesetterCreateWithAttributedString(attrString);
	auto frame = dl_CTFramesetterCreateFrame(framesetter, dl_CFRangeMake(0, 0), path, NULL);

	// draw
	dl_CTFrameDraw(frame, context);

	// release
	dl_CFRelease(frame);
	dl_CFRelease(framesetter);
	dl_CFRelease(attrString);
}

void doBigLabel(dl_CGContextRef c, int width, int height) {
	// big label
	auto bigLabelText = dl_CFSTR("BIG-LABEL");
	auto helv120 = dl_CTFontCreateWithName(dl_CFSTR(HELVETICA_FONT_NAME), 120.0, NULL);

	dl_CFStringRef keys[] = {
		dl_kCTFontAttributeName,
		dl_kCTStrokeWidthAttributeName,
		dl_kCTStrokeColorAttributeName,
		dl_kCTForegroundColorAttributeName,
	};
	dl_CFTypeRef values[] = {
		helv120,
		dl_CFNumberWithFloat(-2.0f), // [NSNumber numberWithFloat:-2.0],
		dl_CGColorCreateGenericRGB(1, 1, 0, 1), //[NSColor colorWithRed:1 green:1 blue:0 alpha:1],
		dl_CGColorCreateGenericRGB(0, 0, 0, 1) //[NSColor colorWithRed:0 green:0 blue:0 alpha:1],
	};
	dl_CFDictionaryRef attrsDict = dl_CFDictionaryCreate((const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]));

	auto labelString = dl_CFAttributedStringCreate(bigLabelText, attrsDict);

	auto line = dl_CTLineCreateWithAttributedString(labelString);

	auto bounds = dl_CTLineGetBoundsWithOptions(line, 0);
	dl_CGFloat ascent, descent, leading;
	dl_CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
	//printf("line bounds: %.2f,%.2f,%.2f,%.2f - descent %.2f\n", bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height, descent);

	dl_CGContextSetTextPosition(c, (width - bounds.size.width) / 2, 10.0 + ascent);

	dl_CTLineDraw(line, c);

	dl_CFRelease(line);

	dl_CFRelease(bigLabelText);
	dl_CFRelease(attrsDict);
	auto attrCount = sizeof(values) / sizeof(dl_CFTypeRef);
	for (size_t i = 0; i < attrCount; i++) {
		dl_CFRelease(values[i]);
	}
	dl_CFRelease(labelString);
}

static void doGradientLabel(dl_CGContextRef c, int width, int height)
{
	auto gradLabelText = dl_CFSTR("GRAD-LABEL");
	auto helv180 = dl_CTFontCreateWithName(dl_CFSTR(HELVETICA_FONT_NAME), 180.0, NULL);
	dl_CFStringRef keys[] = {
		dl_kCTFontAttributeName
	};
	dl_CFTypeRef values[] = {
		helv180
	};

	dl_CFDictionaryRef attrsDict = dl_CFDictionaryCreate((const void **)&keys, (const void **)&values, sizeof(keys) / sizeof(keys[0]));
	auto labelString = dl_CFAttributedStringCreate(gradLabelText, attrsDict);

	auto line = dl_CTLineCreateWithAttributedString(labelString);
	auto bounds = dl_CTLineGetBoundsWithOptions(line, dl_kCTLineBoundsUseGlyphPathBounds);

	dl_CGFloat ascent, descent, leading;
	dl_CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
	auto x = (width - bounds.size.width) / 2;
	auto y = height - (leading + 10);

	dl_CGContextSetTextPosition(c, x, y);

	// begin clip ====>
	dl_CGContextSaveGState(c);
	dl_CGContextSetTextDrawingMode(c, dl_kCGTextClip);
	dl_CTLineDraw(line, c);

	// now do gradient inside of clip
	auto grad = getGradient(0, 0, 0, 1, 0, 1, 1, 1);
	auto start = dl_CGPointMake(x, y + bounds.origin.y); // top left of bounds rect (y is negative, relative to baseline)
	auto end = dl_CGPointMake(x, y + bounds.origin.y + bounds.size.height); // bottom left of bounds rect
	dl_CGContextDrawLinearGradient(c, grad, start, end, dl_kCGGradientDrawsBeforeStartLocation | dl_kCGGradientDrawsAfterEndLocation);
	dl_CGGradientRelease(grad);

	dl_CGContextRestoreGState(c);
	// <==== end clip

	// create image to use for masking
	auto graySpace = dl_CGColorSpaceCreateDeviceGray();
	auto bmContext = dl_CGBitmapContextCreate(nullptr, (size_t)width, (size_t)height, 8, (size_t)width, graySpace, dl_kCGBitmapByteOrderDefault);
	auto black = dl_CGColorGetConstantColor(dl_kCGColorBlack);
	dl_CGContextSetFillColorWithColor(bmContext, black);
	dl_CGContextFillRect(bmContext, dl_CGRectMake(0, 0, width, height));

	// draw outside stroke on mask
	dl_CGContextSetTextPosition(bmContext, x, y);
	dl_CGContextSetTextDrawingMode(bmContext, dl_kCGTextStroke);
	auto white = dl_CGColorGetConstantColor(dl_kCGColorWhite);
	dl_CGContextSetStrokeColorWithColor(bmContext, white);
	dl_CGContextSetLineWidth(bmContext, 4.0f);
	dl_CTLineDraw(line, bmContext);

	// apply bitmap as mask ===>
	dl_CGContextSaveGState(c);

	auto imageRef = dl_CGBitmapContextCreateImage(bmContext);
	dl_CGContextClipToMask(c, dl_CGRectMake(0, 0, width, height), imageRef);

	// fill 2nd gradient over masked outline
	auto grad2 = getGradient(1, 0.5, 0, 1, 0, 0, 0, 1);
	dl_CGContextDrawLinearGradient(c, grad2, start, end, dl_kCGGradientDrawsBeforeStartLocation | dl_kCGGradientDrawsAfterEndLocation);
	dl_CGGradientRelease(grad2);

	dl_CGContextRestoreGState(c);
	// <=== end mask

	// release
	dl_CGImageRelease(imageRef);
	//dl_CFRelease(black);  // don't release constant colors!
	//dl_CFRelease(white);
    dl_CGContextRelease(bmContext);
	dl_CGColorSpaceRelease(graySpace);
	dl_CFRelease(line);
	dl_CFRelease(labelString);

	dl_CFRelease(gradLabelText);
	dl_CFRelease(helv180);
	dl_CFRelease(attrsDict);
}

class CPage06 : public PageCommon {
public:
	CPage06(WindowFuncs *parent) 
		: PageCommon(parent) {}
	void render(dl_CGContextRef c, int width, int height) override {
		// gradient for fun
		auto grad = getGradient(0, 0.5, 1, 1, 0, 0, 0, 1);
		dl_CGContextDrawLinearGradient(c, grad, dl_CGPointMake(0, height), dl_CGPointZero, 0);
		dl_CGGradientRelease(grad);

		//    // need to flip the coords since we're top-down by default, but text rendering is bottom-up
		//    dl_CGContextTranslateCTM(c, 0, height);
		//    dl_CGContextScaleCTM(c, 1.0, -1.0);

		dl_CGContextSetTextMatrix(c, dl_CGAffineTransformIdentity);

		auto textString = dl_CFSTR("Vestibulum mattis ipsum augue, ac maximus erat scelerisque vitae. Nam vel commodo urna, a consectetur arcu. Quisque a suscipit metus. Nam bibendum accumsan vulputate. Proin suscipit, sapien a fermentum laoreet, est turpis sollicitudin ipsum, quis consectetur purus augue non ante. Donec sollicitudin erat sed urna posuere vehicula. Sed lacus felis, dapibus sed leo vel, pretium gravida lectus. Fusce id eros euismod, cursus ante sit amet, tempus tortor. Aenean venenatis quam ut tortor aliquet ornare. Curabitur sollicitudin molestie nisi, sit amet sollicitudin tortor maximus at. Phasellus eget vehicula ligula. Ut eu tempus urna. Donec tempor velit in odio vulputate, id condimentum justo finibus. Donec cursus nulla et pellentesque finibus. Phasellus tincidunt ligula pellentesque pulvinar ultrices. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Sed malesuada mauris ac tortor consequat gravida et non urna. Interdum et malesuada fames ac ante ipsum primis in faucibus. Proin vitae justo erat. Suspendisse potenti. Sed sit amet leo in sapien placerat maximus vitae eget tellus. Etiam ullamcorper augue eros, at mollis libero placerat id. Nulla id suscipit quam, non rhoncus tellus. Nulla mauris elit, congue id accumsan in, pellentesque ullamcorper nisi. In a elit mi. Quisque feugiat justo at ipsum lacinia finibus.");

		// colors
		auto white = dl_CGColorCreateGenericRGB(1, 1, 1, 1);
		auto black = dl_CGColorCreateGenericRGB(0, 0, 0, 1);
		auto green = dl_CGColorCreateGenericRGB(0, 1, 0, 1);
		auto yellow = dl_CGColorCreateGenericRGB(1, 1, 0, 1);

		// font
		auto times40 = dl_CTFontCreateWithName(dl_CFSTR(TIMES_FONT_NAME), 40.0, NULL);
		auto helv16 = dl_CTFontCreateWithName(dl_CFSTR(HELVETICA_FONT_NAME), 16.0, NULL);
		auto futura32 = dl_CTFontCreateWithName(dl_CFSTR(FUTURA_FONT_NAME), 32.0, NULL);

		// shapes
		auto rect = dl_CGPathCreateWithRect(dl_CGRectMake(10, 10, width - 20, height - 20), NULL); // whole window (minus borders)
		auto rect2 = dl_CGPathCreateWithRect(dl_CGRectMake(width - 400, 0, 400, height), NULL); // right side (top to bottom)
		auto rect3 = dl_CGPathCreateWithRect(dl_CGRectMake(50, (height - 300) / 2, 300, 300), NULL); // mid-left-side, 300px wide/tall

		//    auto minRad = fmin(width, height);
		//    auto exclRad = fmin(200, minRad/4);
		//    auto disc = CGPathCreateMutable();
		//    CGPathAddArc(disc, NULL, width/2, height/2, minRad/2, 0, M_PI * 2.0, NO);
		//    CGPathCloseSubpath(disc);
		//    CGPathMoveToPoint(disc, NULL, width/2 + exclRad, height/2);
		//    CGPathAddArc(disc, NULL, width/2, height/2, exclRad, 0, M_PI * 2, NO); // inner exclusion
		//    CGPathCloseSubpath(disc);

		// text drawing
		drawTextInShape(c, height, rect, textString, black, times40);

		dl_CGContextSetRGBStrokeColor(c, 1, 0, 0, 1);
		dl_CGContextAddPath(c, rect3);
		dl_CGContextStrokePath(c);

		auto justified = dl_kCTTextAlignmentJustified;
		drawTextInShape(c, height, rect3, textString, white, helv16, &justified);

		drawTextInShape(c, height, rect2, textString, green, futura32);

		doBigLabel(c, width, height);

		//// bottom gradient
		//auto darkGrad = getGradient(0, 0, 0, 1, 0, 0, 0, 0); // opaque black to transparent
		//dl_CGContextDrawLinearGradient(c, darkGrad,
		//    dl_CGPointMake(0, height),
		//    dl_CGPointMake(0, height - 200), 0);

		doGradientLabel(c, width, height);

		// release
		dl_CFRelease(rect);
		dl_CFRelease(rect2);
		dl_CFRelease(rect3);

		dl_CFRelease(yellow);
		dl_CFRelease(green);
		dl_CFRelease(black);
		dl_CFRelease(white);

		dl_CFRelease(futura32);
		dl_CFRelease(times40);
		dl_CFRelease(helv16);

		dl_CFRelease(textString);
	}
};

#endif
