#ifndef __COMMON_H__
#define __COMMON_H__

#include "../../../source/opendl.h"
#include "../deps/openwl/source/openwl.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef DL_PLATFORM_WINDOWS
#   define HELVETICA_FONT_NAME "Arial"
#   define TIMES_FONT_NAME "Times New Roman"
#   define TIMES_ITALIC_FONT_NAME "TimesNewRomanPS-ItalicMT" // currently a hardcoded switch in the font ctor, need to create a proper PS name mapping
#   define FUTURA_FONT_NAME "Impact"
#elif defined DL_PLATFORM_APPLE
#   define HELVETICA_FONT_NAME "Helvetica"
#   define TIMES_FONT_NAME "TimesNewRomanPSMT"
#   define TIMES_ITALIC_FONT_NAME "TimesNewRomanPS-ItalicMT"
#   define FUTURA_FONT_NAME "Futura-Medium"
#elif defined DL_PLATFORM_LINUX
#   define HELVETICA_FONT_NAME "Liberation Sans"
#   define TIMES_FONT_NAME "Liberation Serif"
#   define TIMES_ITALIC_FONT_NAME "Liberation Serif Italic"
#   define FUTURA_FONT_NAME "Liberation Sans"
#endif

void dashedRect(dl_CGContextRef c, dl_CGRect r);

dl_CGGradientRef getGradient(dl_CGFloat fromR, dl_CGFloat fromG, dl_CGFloat fromB, dl_CGFloat fromA,
	dl_CGFloat toR, dl_CGFloat toG, dl_CGFloat toB, dl_CGFloat toA);

void gradientDashedRect(dl_CGContextRef c, dl_CGRect r);
void textLine(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y, dl_CFStringRef text, dl_CFDictionaryRef attrsDict, bool withGradient = true, bool fromBaseline = false);

void pointAt(dl_CGContextRef c, dl_CGPoint p);
void drawRect(dl_CGContextRef c, dl_CGRect r, dl_CGColorRef color, dl_CGFloat width);
void drawLine(dl_CGContextRef c, dl_CGPoint from, dl_CGPoint to, dl_CGColorRef color);
void drawLineAt(dl_CGContextRef c, dl_CTLineRef line, dl_CGFloat x, dl_CGFloat y, dl_CGFloat *y_advance);

class WindowFuncs {
public:
	virtual void invalidate(int x, int y, int width, int height) = 0;
};

class PageCommon {
protected:
	WindowFuncs *parent;
public:
	PageCommon(WindowFuncs *parent)
		:parent(parent) {}

	virtual void init() {} // can't use constructors because probably nothing we want to use (writeFactory etc) is ready
	virtual void render(dl_CGContextRef context, int width, int height) = 0;
	virtual void render2(dl_CGContextRef context, int width, int height) {
		// useful for A/B comparisons
		render(context, width, height);
	}
	virtual bool isAnimating() { return false; }
	virtual void onTimerEvent(double secondsSinceLast) {}
	virtual void onMouseEvent(wl_MouseEvent &mouseEvent) {}
	virtual void onKeyEvent(wl_KeyEvent &keyEvent) {}
	virtual void onSizeEvent(wl_ResizeEvent &resizeEvent) {}
};

extern const char *loremIpsum;
extern const char *arabicSample;

#endif

