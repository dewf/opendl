#ifndef __PAGE08_HPP__
#define __PAGE08_HPP__

#include "common.h"

#include <algorithm>

//bool rectContainsPoint(dl_CGRect r, dl_CGPoint p) {
//    if (p.x >= r.origin.x && p.y >= r.origin.y &&
//        p.x < (r.origin.x + r.size.width) &&
//        p.y < (r.origin.y + r.size.height))
//    {
//        return true;
//    }
//    else {
//        return false;
//    }
//}

bool rectContainsPoint(dl_CGRect r, dl_CGPoint p) {
    return p.x >= r.origin.x && p.y >= r.origin.y &&
        p.x < (r.origin.x + r.size.width) &&
        p.y < (r.origin.y + r.size.height);
}

dl_CFAttributedStringRef createAttrString(dl_CTFontRef font, const char *utf8text) {
    auto dl_CFSTR = dl_CFStringCreateWithCString(utf8text);
    auto attrs = dl_CFDictionaryCreateMutable(0);
    dl_CFDictionarySetValue(attrs, dl_kCTFontAttributeName, font);
    dl_CFDictionarySetValue(attrs, dl_kCTForegroundColorAttributeName, dl_CGColorGetConstantColor(dl_kCGColorBlack));
    
    //dl_CTParagraphStyleSetting pss;
    //pss.spec = dl_kCTParagraphStyleSpecifierAlignment;
    //auto value = dl_kCTTextAlignmentNatural;
    //pss.value = &value;
    //pss.valueSize = sizeof(value);
    //auto style = dl_CTParagraphStyleCreate(&pss, 1);
    //dl_CFDictionarySetValue(attrs, dl_kCTParagraphStyleAttributeName, style);
    //dl_CFRelease(style);

    auto attrString = dl_CFAttributedStringCreate(dl_CFSTR, attrs);
    dl_CFRelease(dl_CFSTR);
    dl_CFRelease(attrs);
    return attrString;
}

class CPage08 : public PageCommon {
	dl_CTFontRef font = nullptr;
	dl_CFAttributedStringRef latinString = nullptr;
    dl_CFAttributedStringRef arabicString = nullptr;
    dl_CFAttributedStringRef currentAttrString = nullptr;
	dl_CTFrameRef frame = nullptr;
	dl_CGPoint mousePos = {};
	dl_CGRect textRect = dl_CGRectZero;

	dl_CFArrayRef lines = nullptr;
	dl_CFIndex numLines = dl_kCFNotFound;
    
	dl_CFIndex mouseIndex = dl_kCFNotFound;
	dl_CFIndex startIndex = dl_kCFNotFound;
	dl_CFIndex endIndex = dl_kCFNotFound;
	bool dragging = false;
    
    enum {
        LeftToRight,
        RightToLeft
    } textDirection;
    
    int viewportHeight = -1;

public:
	CPage08(WindowFuncs *parent)
		: PageCommon(parent) {}

	void init() override {
		font = dl_CTFontCreateWithName(dl_CFSTR(TIMES_FONT_NAME), 36, NULL);
        
        latinString = createAttrString(font, loremIpsum);
        arabicString = createAttrString(font, arabicSample);
        currentAttrString = latinString;
        textDirection = LeftToRight;
	}
    
    void doLayout(int height) {
        auto fs = dl_CTFramesetterCreateWithAttributedString(currentAttrString, height);
        auto path = dl_CGPathCreateWithRect(textRect, nullptr);
        auto frameAttrs = dl_CFDictionaryCreate(nullptr, nullptr, 0);
        
        if (frame) dl_CFRelease(frame);
        frame = dl_CTFramesetterCreateFrame(fs, dl_CFRangeZero, path, frameAttrs);
        
        lines = dl_CTFrameGetLines(frame);
        numLines = dl_CFArrayGetCount(lines);
        
        dl_CFRelease(frameAttrs);
        dl_CFRelease(path);
        dl_CFRelease(fs);
    }
    
    void onKeyEvent(wl_KeyEvent &keyEvent) override {
        if (keyEvent.eventType == wl_kKeyEventTypeDown) {
            if (keyEvent.key == wl_kKeySpace) {
                if (currentAttrString == latinString) {
                    currentAttrString = arabicString;
                    textDirection = RightToLeft;
                } else {
                    currentAttrString = latinString;
                    textDirection = LeftToRight;
                }
                doLayout(viewportHeight);
                parent->invalidate(0, 0, 0, 0);
            }
        }
    }

	void onSizeEvent(wl_ResizeEvent &resizeEvent) override {
        auto width = resizeEvent.newWidth;
        viewportHeight = resizeEvent.newHeight;
        textRect = dl_CGRectMake(20.5, 20.5, width - 39, viewportHeight - 39);
        
        doLayout(viewportHeight);
	}

	void onMouseEvent(wl_MouseEvent &mouseEvent) override {
		mousePos = dl_CGPointMake(mouseEvent.x, mouseEvent.y);

		if (mouseEvent.eventType == wl_kMouseEventTypeMouseMove) {
			// calculate the cursor position (line/col etc)
			auto origins = new dl_CGPoint[numLines];
			dl_CTFrameGetLineOrigins(frame, dl_CFRangeZero, origins);

			mouseIndex = dl_kCFNotFound;
			for (dl_CFIndex i = 0; i < numLines; i++) {
				auto line = (dl_CTLineRef)dl_CFArrayGetValueAtIndex(lines, i);
				auto bounds = dl_CTLineGetBoundsWithOptions(line, 0);
				//printf("origins.x: %.2f, bounds x: %.2f\n", origins[i].x, bounds.origin.x);
				auto absBounds = bounds;
				absBounds.origin.x += origins[i].x;
				absBounds.origin.y += origins[i].y;
				if (rectContainsPoint(absBounds, mousePos))
				{
					// cursor location
					auto hitPos = mousePos;
					// point must be relative to line origin
                    hitPos.x -= origins[i].x;
                    hitPos.y -= origins[i].y;
					auto index = dl_CTLineGetStringIndexForPosition(line, hitPos);
					if (index != dl_kCFNotFound) {
						mouseIndex = index;
						if (dragging) {
							endIndex = mouseIndex;
						}
					}
					break;
				}
			}
			delete[] origins;
		} else if (mouseEvent.eventType == wl_kMouseEventTypeMouseDown) {
			// already know what line/index it is (if any)
			if (mouseIndex >= 0) {
				// begin selection drag
				startIndex = mouseIndex;
				dragging = true;
			}
		} else if (mouseEvent.eventType == wl_kMouseEventTypeMouseUp) {
			startIndex = dl_kCFNotFound;
			endIndex = dl_kCFNotFound;
			dragging = false;
        } else if (mouseEvent.eventType == wl_kMouseEventTypeMouseLeave) {
            mouseIndex = dl_kCFNotFound;
        }

		parent->invalidate(0, 0, 0, 0);
	}
    
	void render(dl_CGContextRef c, int width, int height) override {
		dl_CGContextSetRGBFillColor(c, 0.5, 0.5, 0.5, 1);
		dl_CGContextFillRect(c, dl_CGRectMake(0, 0, width, height));

		dl_CGContextSetRGBStrokeColor(c, 1, 1, 0, 1);
		dl_CGContextSetLineWidth(c, 1.0);
		dl_CGContextStrokeRect(c, textRect);
        
        dl_CGContextSetTextMatrix(c, dl_CGAffineTransformIdentity);
        
        auto origins = new dl_CGPoint[numLines];
        dl_CTFrameGetLineOrigins(frame, dl_CFRangeZero, origins);
        
        auto useStartIndex = startIndex;
        auto useEndIndex = endIndex;
        if (useStartIndex > useEndIndex) std::swap(useStartIndex, useEndIndex);

        for (dl_CFIndex i = 0; i < numLines; i++) {
            auto line = (dl_CTLineRef) dl_CFArrayGetValueAtIndex(lines, i);
            auto lineRange = dl_CTLineGetStringRange(line);
            auto bounds = dl_CTLineGetBoundsWithOptions(line, 0);
            auto absBounds = bounds;
            absBounds.origin.x = origins[i].x + bounds.origin.x;
            absBounds.origin.y = origins[i].y + bounds.origin.y;

            dl_CGContextSetRGBFillColor(c, 1, 0, 0, 1);
            dl_CGContextAddArc(c, origins[i].x, origins[i].y, 2, 0, M_PI * 2, 0);
            dl_CGContextDrawPath(c, dl_kCGPathFill);

            // highlight line if mouse inside
            if (rectContainsPoint(absBounds, mousePos)) {
                dl_CGContextSetRGBFillColor(c, 0, 0.8, 1, 0.25);
                dl_CGContextFillRect(c, absBounds);
            }
            
            // range highlighting
            if (useStartIndex >= 0 && useEndIndex >= 0 &&
                dl_CFRangeEnd(lineRange) > useStartIndex &&
                lineRange.location < useEndIndex)
            {
                auto clamped = bounds;
                
                // at least some portion of this line needs to be highlighted
                if (useStartIndex >= lineRange.location && useStartIndex < dl_CFRangeEnd(lineRange)) {
                    auto xOffs = dl_CTLineGetOffsetForStringIndex(line, useStartIndex, nullptr);
                    if (textDirection == RightToLeft) {
                        // clamp rectangle end to xoffs
                        clamped.size.width = xOffs - clamped.origin.x;
                    } else {
                        // clamp rectangle start to xoffs
                        auto end = clamped.origin.x + clamped.size.width;
                        clamped.origin.x = xOffs;
                        clamped.size.width = end - xOffs; //-= xOffs;
                    }
                }
                
                if (useEndIndex >= lineRange.location && useEndIndex < dl_CFRangeEnd(lineRange)) {
                    auto xOffs = dl_CTLineGetOffsetForStringIndex(line, useEndIndex, nullptr);
                    if (textDirection == RightToLeft) {
                        // clamp rectangle start to xoffs
                        auto end = clamped.origin.x + clamped.size.width;
                        clamped.origin.x = xOffs;
                        clamped.size.width = end - xOffs; //-= xOffs;
                    } else {
                        // clamp rectangle end to xoffs
                        clamped.size.width = xOffs - clamped.origin.x;
                    }
                }
                
                clamped.origin.x += origins[i].x;
                clamped.origin.y += origins[i].y;
                
                dl_CGContextSetRGBFillColor(c, 1, 1, 0, 1);
                dl_CGContextFillRect(c, clamped);
            }
            
            // draw cursor position
            if (mouseIndex >= lineRange.location && mouseIndex < dl_CFRangeEnd(lineRange)) {
                auto offs = dl_CTLineGetOffsetForStringIndex(line, mouseIndex, nullptr);
                auto cursorXOffs = origins[i].x + offs;
                dl_CGContextMoveToPoint(c, cursorXOffs, absBounds.origin.y);
                dl_CGContextAddLineToPoint(c, cursorXOffs, absBounds.origin.y + absBounds.size.height);
                dl_CGContextSetLineWidth(c, 2);
                dl_CGContextSetRGBStrokeColor(c, 1, 1, 0, 1);
                dl_CGContextStrokePath(c);
            }
        }
        delete[] origins;
        
		dl_CTFrameDraw(frame, c);
	}
};


#endif
