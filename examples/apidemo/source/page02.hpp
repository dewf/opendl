#ifndef __PAGE02_HPP__
#define __PAGE02_HPP__

#include "common.h"

class CPage02 : public PageCommon {
    dl_CTFontRef serif = nullptr;
public:
    CPage02(WindowFuncs *parent)
            : PageCommon(parent) {}

    void init() override
    {
        //times = dl_CTFontCreateWithName(dl_CFSTR(TIMES_FONT_NAME), 120, nullptr);
        auto path = dl_CFSTR("./_democontent/LiberationSerif-Regular.ttf");
        auto url = dl_CFURLCreateWithFileSystemPath(path, dl_kCFURLPOSIXPathStyle, false);
        auto fdArray = dl_CTFontManagerCreateFontDescriptorsFromURL(url);
        if (fdArray && dl_CFArrayGetCount(fdArray) > 0) {
            // now try to create a font face from the first one
            auto fd = (dl_CTFontDescriptorRef)dl_CFArrayGetValueAtIndex(fdArray, 0);
            serif = dl_CTFontCreateWithFontDescriptor(fd, 120, NULL);
            // don't release fd, it was never retained separately from the array!
            dl_CFRelease(fdArray);
        }
        dl_CFRelease(url);
        dl_CFRelease(path);
    }

    void render(dl_CGContextRef c, int width, int height) override {

        dl_CGContextSetRGBFillColor(c, 0.2, 0.2, 0.3, 1);
        dl_CGContextFillRect(c, dl_CGRectMake(0, 0, width, height));
        dl_CGContextSetTextMatrix(c, dl_CGAffineTransformIdentity);

        //auto text = dl_CFStringCreateWithCString(loremIpsum);
        auto text = dl_CFSTR("Quartz♪❦♛あぎ");//♪❦♛あぎ");

        auto dict = dl_CFDictionaryCreateMutable(0);
        dl_CFDictionarySetValue(dict, dl_kCTFontAttributeName, serif);
        auto fgColor = dl_CGColorCreateGenericRGB(0, 1, 1, 0.5);
        dl_CFDictionarySetValue(dict, dl_kCTForegroundColorAttributeName, fgColor);
        auto labelString = dl_CFAttributedStringCreate(text, dict);

        auto line = dl_CTLineCreateWithAttributedString(labelString);

        dl_CGFloat x, y, y_advance;
        x = 100;
        y = 200;

        drawLineAt(c, line, x, y, &y_advance);

        dl_CFRelease(line);

        // concentric circles to test arcs
        dl_CGContextSetRGBStrokeColor(c, 0, 0.4, 1, 1);
        dl_CGContextSetLineWidth(c, 2.0);
        x = width / 2.0;
        y = (height * 2.0) / 3.0;
        //dl_CGContextMoveToPoint(c, x, y);
        dl_CGContextBeginPath(c);
        dl_CGContextAddArc(c, x, y, 240, 0, 2 * M_PI, 1);
        dl_CGContextStrokePath(c);

        dl_CGContextSetLineWidth(c, 10);
        dl_CGFloat radius = 230;
        dl_CGFloat startAngle = 0;
        for (int i = 0; i < 10; i++) {
            auto endAngle = startAngle + M_PI;

            dl_CGContextBeginPath(c);
            dl_CGContextAddArc(c, x, y, radius, startAngle, endAngle, 1);
            dl_CGContextSetRGBStrokeColor(c, 0, 0.6, 1, 1);
            dl_CGContextStrokePath(c);

            startAngle += M_PI;
            endAngle += M_PI;
            dl_CGContextBeginPath(c);
            dl_CGContextAddArc(c, x, y, radius, startAngle, endAngle, 1);
            dl_CGContextSetRGBStrokeColor(c, 0, 0, 0, 1);
            dl_CGContextStrokePath(c);

            startAngle += M_PI + M_PI / 6;
            radius -= 15;
        }
    }
};

#endif
