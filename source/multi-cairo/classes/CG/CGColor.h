#ifndef __CG_COLOR_H__
#define __CG_COLOR_H__

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"

class CGColor; typedef CGColor* CGColorRef;
class CGColor : public cf::Object {
    dl_CGFloat red, green, blue, alpha;
    dl_CGFloat components[4]; // for returning as a dl_CGFloat*, otherwise not used

public:
    static CGColorRef White;
    static CGColorRef Black;
    static CGColorRef Clear;

    const char *getTypeName() const override {
        return "CGColor";
    }

    CGColor() {}

    CGColor(const CGColor& other) {
        *this = other; // just copy fields directly
    }

    CGColor(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha = 1.0)
        :red(red), green(green), blue(blue), alpha(alpha){}

    inline dl_CGFloat getRed() { return red; }

    inline dl_CGFloat getGreen() { return green; }

    inline dl_CGFloat getBlue() { return blue; }

    inline dl_CGFloat getAlpha() { return alpha; }

    void apply(cairo_t *cr);

    CGColorRef copy() override {
        // immutable so just retain
        return (CGColorRef)retain();
    }

    size_t getNumberOfComponents();

    const dl_CGFloat *getComponents();
};

#endif
