#ifndef __PAGE05_HPP__
#define __PAGE05_HPP__

#include "common.h"

// font page 02 stuff ==============================

dl_CFStringRef kDLBackgroundAttributeName = dl_CFSTR("kDLBackgroundAttributeName");
dl_CFStringRef kDLHighlightAttributeName = dl_CFSTR("kDLHighlightAttributeName");
dl_CFStringRef kDLStrikeCountAttributeName = dl_CFSTR("kDLStrikeCountAttributeName");
dl_CFStringRef kDLStrikeColorAttributeName = dl_CFSTR("kDLStrikeColorAttributeName");
dl_CFStringRef kDLUnderlineStyleAttributeName = dl_CFSTR("kDLUnderlineStyleAttributeName"); // CFNumber encoding of UnderlineStyle
dl_CFStringRef kDLUnderlineColorAttributeName = dl_CFSTR("kDLUnderlineColorAttributeName");

typedef enum {
	Single,
	Double,
	Triple,
	Squiggly,
	Overline
} UnderlineStyle;

typedef enum {
	Background,
	Overlay
} EffectLayer;

static void drawFrameEffects(dl_CGContextRef context, dl_CTFrameRef frame, dl_CGRect rect, EffectLayer layer, dl_CGColorRef defaultColor)
{
	dl_CFArrayRef lines = dl_CTFrameGetLines(frame);
	dl_CFIndex numLines = dl_CFArrayGetCount(lines);
	auto origins = new dl_CGPoint[numLines];
	dl_CTFrameGetLineOrigins(frame, dl_CFRangeZero, origins); // zero length range = fetch all

	for (dl_CFIndex lineIndex = 0; lineIndex < numLines; lineIndex++) {
		// illustrate origins =================
		auto p = origins[lineIndex];
		dl_CGContextSetRGBFillColor(context, 1, 0, 0, 1);
		dl_CGContextFillRect(context, dl_CGRectMake(p.x - 2, p.y - 2, 4, 4));
		// end=================================

		auto line = (dl_CTLineRef)dl_CFArrayGetValueAtIndex(lines, lineIndex);
		//
		dl_CGFloat ascent, descent, leading;
		dl_CGFloat width = dl_CTLineGetTypographicBounds(line, &ascent, &descent, &leading);

		dl_CFArrayRef runs = dl_CTLineGetGlyphRuns(line);
		dl_CFIndex numRuns = dl_CFArrayGetCount(runs);
		for (dl_CFIndex runIndex = 0; runIndex < numRuns; runIndex++) {
			auto run = (dl_CTRunRef)dl_CFArrayGetValueAtIndex(runs, runIndex);
			dl_CFDictionaryRef attrs = dl_CTRunGetAttributes(run);

			auto font = (dl_CTFontRef)dl_CFDictionaryGetValue(attrs, dl_kCTFontAttributeName);
			auto fgColor = (dl_CGColorRef)dl_CFDictionaryGetValue(attrs, dl_kCTForegroundColorAttributeName);
			auto bgColor = (dl_CGColorRef)dl_CFDictionaryGetValue(attrs, kDLBackgroundAttributeName);
			auto highColor = (dl_CGColorRef)dl_CFDictionaryGetValue(attrs, kDLHighlightAttributeName);
			auto strikeCount = (dl_CFNumberRef)dl_CFDictionaryGetValue(attrs, kDLStrikeCountAttributeName);
			auto strikeColor = (dl_CGColorRef)dl_CFDictionaryGetValue(attrs, kDLStrikeColorAttributeName);
			auto underStyle = (dl_CFNumberRef)dl_CFDictionaryGetValue(attrs, kDLUnderlineStyleAttributeName);
			auto underColor = (dl_CGColorRef)dl_CFDictionaryGetValue(attrs, kDLUnderlineColorAttributeName);

			dl_CGRect runBounds = dl_CGRectZero;
			dl_CGFloat runAscent, runDescent, runLeading;

			// might want to pad these with user-definable pads?
			runBounds.size.width = dl_CTRunGetTypographicBounds(run, dl_CFRangeMake(0, 0), &runAscent, &runDescent, &runLeading);
			runBounds.size.height = runAscent + runDescent;

			dl_CGFloat xOffset = 0.0f;
			dl_CFRange glyphRange = dl_CTRunGetStringRange(run);
			dl_CTRunStatus status = dl_CTRunGetStatus(run);
			if (status == dl_kCTRunStatusRightToLeft) {
				xOffset = dl_CTLineGetOffsetForStringIndex(line, glyphRange.location + glyphRange.length, NULL);
			}
			else {
				xOffset = dl_CTLineGetOffsetForStringIndex(line, glyphRange.location, NULL);
			}

			runBounds.origin.x = origins[lineIndex].x + xOffset;
			runBounds.origin.y = origins[lineIndex].y;
			runBounds.origin.y -= runAscent;

			if (dl_CGRectGetWidth(runBounds) > width) {
				runBounds.size.width = width;
			}

			if (layer == Background) {
				if (bgColor) {
					dl_CGContextSetFillColorWithColor(context, bgColor);
					dl_CGContextFillRect(context, runBounds);
				}
				if (underStyle) {
					UnderlineStyle ulStyle;
					dl_CFNumberGetValue(underStyle, dl_kCFNumberIntType, &ulStyle);
					dl_CGColorRef ulColor = underColor ? underColor : (fgColor ? fgColor : (defaultColor));
					dl_CGContextSetStrokeColorWithColor(context, ulColor);
					//
					dl_CGFloat thickness = dl_CTFontGetUnderlineThickness(font);

					dl_CGContextSetLineWidth(context, thickness);
					//
					dl_CGFloat y = origins[lineIndex].y + dl_CTFontGetUnderlinePosition(font);
					//runBounds.origin.y + runDescent + dl_CTFontGetUnderlinePosition(font);

					if (ulStyle >= Single && ulStyle <= Triple) {
						dl_CGContextMoveToPoint(context, runBounds.origin.x, y);
						dl_CGContextAddLineToPoint(context, runBounds.origin.x + runBounds.size.width, y);
						dl_CGContextStrokePath(context);
						if (ulStyle >= Double) {
							y += thickness * 2;
							dl_CGContextMoveToPoint(context, runBounds.origin.x, y);
							dl_CGContextAddLineToPoint(context, runBounds.origin.x + runBounds.size.width, y);
							dl_CGContextStrokePath(context);
							if (ulStyle >= Triple) {
								y += thickness * 2;
								dl_CGContextMoveToPoint(context, runBounds.origin.x, y);
								dl_CGContextAddLineToPoint(context, runBounds.origin.x + runBounds.size.width, y);
								dl_CGContextStrokePath(context);
							}
						}
					}
					else if (ulStyle == Overline) {
						y -= runAscent - thickness; // thickness hopefully being proportional to font size ...
						dl_CGContextMoveToPoint(context, runBounds.origin.x, y);
						dl_CGContextAddLineToPoint(context, runBounds.origin.x + runBounds.size.width, y);
						dl_CGContextStrokePath(context);
					}
					else if (ulStyle == Squiggly) {
						dl_CGFloat amplitude = thickness;
						dl_CGFloat period = 5 * thickness;

						for (int t = 0; t < runBounds.size.width; t++) {
							dl_CGFloat px = runBounds.origin.x + t;
							dl_CGFloat angle = M_PI * 2.0 * fmod(px, period) / period;
							dl_CGFloat py = y + sin(angle) * amplitude;
							if (t == 0) {
								dl_CGContextMoveToPoint(context, px, py);
							}
							else {
								dl_CGContextAddLineToPoint(context, px, py);
							}
						}
						dl_CGContextStrokePath(context);
					}
				}
			}
			else if (layer == Overlay) {
				if (strikeCount) {
					int theCount;
					dl_CFNumberGetValue(strikeCount, dl_kCFNumberIntType, &theCount);
					dl_CGColorRef useColor = strikeColor ? strikeColor : (fgColor ? fgColor : (defaultColor));
					dl_CGContextSetStrokeColorWithColor(context, useColor);
					//
					dl_CGFloat thickness = dl_CTFontGetUnderlineThickness(font);
					dl_CGContextSetLineWidth(context, thickness);
					//
					dl_CGFloat y = origins[lineIndex].y - (runAscent / 4.0f);
					//runBounds.origin.y + runDescent + (runAscent / 4.0f);
				//
					if (theCount == 1 || theCount == 3) {
						dl_CGContextMoveToPoint(context, runBounds.origin.x, y);
						dl_CGContextAddLineToPoint(context, runBounds.origin.x + runBounds.size.width, y);
						dl_CGContextStrokePath(context);
					}
					if (theCount == 2 || theCount == 3) {
						auto y2 = y - (thickness * (theCount - 1));
						dl_CGContextMoveToPoint(context, runBounds.origin.x, y2);
						dl_CGContextAddLineToPoint(context, runBounds.origin.x + runBounds.size.width, y2);
						dl_CGContextStrokePath(context);
						y2 = y + (thickness * (theCount - 1));
						dl_CGContextMoveToPoint(context, runBounds.origin.x, y2);
						dl_CGContextAddLineToPoint(context, runBounds.origin.x + runBounds.size.width, y2);
						dl_CGContextStrokePath(context);
					}
				}
				if (highColor) {
					dl_CGContextSetFillColorWithColor(context, highColor);
					dl_CGContextFillRect(context, runBounds);
				}
			}
		}
	}
	delete[] origins;
}

class CPage05 : public PageCommon {
public:
	CPage05(WindowFuncs *parent) 
		: PageCommon(parent) {}

	void render(dl_CGContextRef c, int width, int height) override {
		dl_CGContextSetRGBFillColor(c, 100 / 255.0, 149 / 255.0, 237 / 255.0, 1);
		dl_CGContextFillRect(c, dl_CGRectMake(0, 0, width, height));

		dl_CGContextSetTextMatrix(c, dl_CGAffineTransformIdentity);

		auto text = dl_CFSTR(
			"This paragraph of text rendered with DirectWrite is based on IDWriteTextFormat and IDWriteTextLayout"
			" objects and uses a custom format specifier passed to the SetDrawingEffect method, and thus is"
			" capable of different RBG RGB foreground colors, such as red, green, and blue as well as double"
			" underline, triple underline, single strikethrough, double strikethrough, triple strikethrough, or combinations thereof."
			" Also possible is something often referred to as an overline, as well as a squiggly (squiggly?) underline.");

		auto timesFont = dl_CTFontCreateWithName(dl_CFSTR(TIMES_FONT_NAME), 40.0, nullptr);
		auto timesItalic = dl_CTFontCreateCopyWithSymbolicTraits(timesFont, 0, nullptr, dl_kCTFontTraitItalic, dl_kCTFontTraitItalic); // 0 = preserve size

		// RECT ===========================
		auto rect = dl_CGRectMake(0, 0, width, height);
		auto rectPath = dl_CGPathCreateWithRect(rect, NULL);
		//    auto rectPath = dl_CGPathCreateWithEllipseInRect(rect, NULL);

		auto black = dl_CGColorCreateGenericRGB(0, 0, 0, 1);
		auto magenta = dl_CGColorCreateGenericRGB(1, 0, 1, 1);
		auto alphaYellow = dl_CGColorCreateGenericRGB(1, 1, 0, 0.5);
		auto red = dl_CGColorCreateGenericRGB(1, 0, 0, 1);
		auto green = dl_CGColorCreateGenericRGB(0, 1, 0, 1);
		auto blue = dl_CGColorCreateGenericRGB(0, 0, 1, 1);
		auto yellow = dl_CGColorCreateGenericRGB(1, 1, 0, 1);

		auto attrString = dl_CFAttributedStringCreateMutable(0); // 0 = amt of storage to reserve, no hint
		dl_CFAttributedStringReplaceString(attrString, dl_CFRangeMake(0, 0), text);
		auto strLen = dl_CFAttributedStringGetLength(attrString);

		// begin batch editing
		dl_CFAttributedStringBeginEditing(attrString);

		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(0, strLen), dl_kCTForegroundColorAttributeName, black);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(0, strLen), dl_kCTFontAttributeName, timesFont);

		dl_CFRange range;

		range = dl_CFStringFind(text, dl_CFSTR("IDWriteTextFormat"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, dl_kCTFontAttributeName, timesItalic);

		range = dl_CFStringFind(text, dl_CFSTR("IDWriteTextLayout"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, dl_kCTFontAttributeName, timesItalic);

		range = dl_CFStringFind(text, dl_CFSTR("IDWriteTextFormat and IDWriteTextLayout objects"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLBackgroundAttributeName, magenta);

		range = dl_CFStringFind(text, dl_CFSTR("the SetDrawingEffect method"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLHighlightAttributeName, alphaYellow);

		range = dl_CFStringFind(text, dl_CFSTR("SetDrawingEffect"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, dl_kCTFontAttributeName, timesItalic);

		range = dl_CFStringFind(text, dl_CFSTR("RBG"), 0);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(range.location, 1), dl_kCTForegroundColorAttributeName, red);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(range.location + 1, 1), dl_kCTForegroundColorAttributeName, blue);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(range.location + 2, 1), dl_kCTForegroundColorAttributeName, green);

		int strikeCount = 1;
		dl_CFNumberRef strike1 = dl_CFNumberCreate(dl_kCFNumberIntType, &strikeCount);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLStrikeCountAttributeName, strike1);

		range = dl_CFStringFind(text, dl_CFSTR("RGB"), 0);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(range.location, 1), dl_kCTForegroundColorAttributeName, red);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(range.location + 1, 1), dl_kCTForegroundColorAttributeName, green);
		dl_CFAttributedStringSetAttribute(attrString, dl_CFRangeMake(range.location + 2, 1), dl_kCTForegroundColorAttributeName, blue);
		UnderlineStyle ulStyle = Single;
		dl_CFNumberRef ulStyleSingle = dl_CFNumberCreate(dl_kCFNumberIntType, &ulStyle);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleSingle);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineColorAttributeName, yellow);

		range = dl_CFStringFind(text, dl_CFSTR(" red"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, dl_kCTForegroundColorAttributeName, red);
		range = dl_CFStringFind(text, dl_CFSTR("green"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, dl_kCTForegroundColorAttributeName, green);
		range = dl_CFStringFind(text, dl_CFSTR("blue"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, dl_kCTForegroundColorAttributeName, blue);

		range = dl_CFStringFind(text, dl_CFSTR("double underline"), 0);
		ulStyle = Double;
		dl_CFNumberRef ulStyleDouble = dl_CFNumberCreate(dl_kCFNumberIntType, &ulStyle);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleDouble);

		range = dl_CFStringFind(text, dl_CFSTR("triple underline"), 0);
		ulStyle = Triple;
		dl_CFNumberRef ulStyleTriple = dl_CFNumberCreate(dl_kCFNumberIntType, &ulStyle);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleTriple);
		//    dl_CFAttributedStringSetAttribute(attrString, range, kDLBackgroundAttributeName, blue);

		range = dl_CFStringFind(text, dl_CFSTR("single strikethrough"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLStrikeCountAttributeName, strike1);

		range = dl_CFStringFind(text, dl_CFSTR("double strikethrough"), 0);
		strikeCount = 2;
		dl_CFNumberRef strike2 = dl_CFNumberCreate(dl_kCFNumberIntType, &strikeCount);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLStrikeCountAttributeName, strike2);

		range = dl_CFStringFind(text, dl_CFSTR("triple strikethrough"), 0);
		strikeCount = 3;
		dl_CFNumberRef strike3 = dl_CFNumberCreate(dl_kCFNumberIntType, &strikeCount);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLStrikeCountAttributeName, strike3);

		range = dl_CFStringFind(text, dl_CFSTR("combinations"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLStrikeCountAttributeName, strike2);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLStrikeColorAttributeName, red);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleTriple);

		range = dl_CFStringFind(text, dl_CFSTR("thereof"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLStrikeCountAttributeName, strike3);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleDouble);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineColorAttributeName, blue);

		range = dl_CFStringFind(text, dl_CFSTR("overline"), 0);
		ulStyle = Overline;
		dl_CFNumberRef ulStyleOverline = dl_CFNumberCreate(dl_kCFNumberIntType, &ulStyle);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleOverline);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineColorAttributeName, blue);

		range = dl_CFStringFind(text, dl_CFSTR("squiggly (squiggly?) underline"), 0);
		ulStyle = Squiggly;
		dl_CFNumberRef ulStyleSquiggly = dl_CFNumberCreate(dl_kCFNumberIntType, &ulStyle);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleSquiggly);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineColorAttributeName, blue);

		range = dl_CFStringFind(text, dl_CFSTR("(squiggly?)"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineColorAttributeName, red);

		range = dl_CFStringFind(text, dl_CFSTR("IDWriteTextLayout"), 0);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineStyleAttributeName, ulStyleSquiggly);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLUnderlineColorAttributeName, blue);
		dl_CFAttributedStringSetAttribute(attrString, range, kDLHighlightAttributeName, alphaYellow);

		// replacement testing =============
		//    range = dl_CFStringFind(text, dl_CFSTR("IDWriteTextLayout obj"), 0);
		//    range.location += 11;
		//    range.length -= 11;
		//    dl_CFAttributedStringReplaceString(attrString, range, dl_CFSTR("xxx"));

		//    range = dl_CFStringFind(text, dl_CFSTR("IDWriteTextLayout"), 0);
		//    dl_CFAttributedStringReplaceString(attrString, range, dl_CFSTR("xxx"));

		//    range = dl_CFStringFind(text, dl_CFSTR("nd IDWrite"), 0);
		//    dl_CFAttributedStringReplaceString(attrString, range, dl_CFSTR("wut"));

		//    range = dl_CFStringFind(text, dl_CFSTR("red, green, and blue"), 0);
		//    dl_CFAttributedStringReplaceString(attrString, range, dl_CFSTR("red, green, and blue"));

		// end batch editing
		dl_CFAttributedStringEndEditing(attrString);

		// framesetter
		auto framesetter = dl_CTFramesetterCreateWithAttributedString(attrString, height);
		auto frame = dl_CTFramesetterCreateFrame(framesetter, dl_CFRangeMake(0, 0), rectPath, NULL);

		// draw background color, underlines
		drawFrameEffects(c, frame, rect, Background, black);

		// draw text + built-in CT effects
		dl_CTFrameDraw(frame, c);

		// draw strikethroughs, highlights
		drawFrameEffects(c, frame, rect, Overlay, black);

		// release
		dl_CFRelease(frame);
		dl_CFRelease(framesetter);

		dl_CFRelease(attrString);

		dl_CFRelease(strike1);
		dl_CFRelease(ulStyleSingle);
		dl_CFRelease(ulStyleDouble);
		dl_CFRelease(ulStyleTriple);
		dl_CFRelease(strike2);
		dl_CFRelease(strike3);
		dl_CFRelease(ulStyleOverline);
		dl_CFRelease(ulStyleSquiggly);

		dl_CGColorRelease(black);
		dl_CGColorRelease(magenta);
		dl_CGColorRelease(alphaYellow);
		dl_CGColorRelease(red);
		dl_CGColorRelease(green);
		dl_CGColorRelease(blue);
		dl_CGColorRelease(yellow);

		dl_CGPathRelease(rectPath);
		dl_CFRelease(timesItalic);
		dl_CFRelease(timesFont);
	}
};

#endif
