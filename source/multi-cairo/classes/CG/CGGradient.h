//
// Created by dang on 9/24/18.
//

#ifndef __CG_GRADIENT_H__
#define __CG_GRADIENT_H__

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"

#include "CGColorSpace.h"

// gradient stuff ==============

struct _gradComponent {
    double loc;
    double r, g, b, a;
};

class CGGradient;

typedef CGGradient *CGGradientRef;

class CGGradient : public cf::Object {
protected:
    _gradComponent *components = nullptr;
    size_t count = 0;
    CGColorSpaceRef space = nullptr;
public:
    const char *getTypeName() const override {
        return "CGGradient";
    }

    CGGradient(CGColorSpaceRef space, const dl_CGFloat components[], const dl_CGFloat locations[], size_t count)
            : space(space->copy()),
              count(count),
              components(new _gradComponent[count]) {
//        this->space = space->copy();
//        this->count = count;
//        this->components = new _gradComponent[count];
        for (size_t i = 0; i < count; i++) {
            _gradComponent &gc = this->components[i];
            gc.loc = locations[i];
            const dl_CGFloat *compBase = &components[i * 4];
            gc.r = compBase[0];
            gc.g = compBase[1];
            gc.b = compBase[2];
            gc.a = compBase[3];
        }
    }

    ~CGGradient() override {
        space->release();
        delete[] components;
    }

    static inline CGGradientRef
    createWithColorComponents(CGColorSpaceRef space, const dl_CGFloat components[], const dl_CGFloat locations[],
                              size_t count)
    {
        return new CGGradient(space, components, locations, count);
    }

    CGGradientRef copy() override {
        // immutable, so retain
        return (CGGradientRef) retain();
    }

    void writeToPattern(cairo_pattern_t *pattern) {
        for (size_t i = 0; i < count; i++) {
            _gradComponent &gc = components[i];
            cairo_pattern_add_color_stop_rgba(pattern, gc.loc, gc.r, gc.g, gc.b, gc.a);
        }
    }
};


#endif //__CG_GRADIENT_H__
