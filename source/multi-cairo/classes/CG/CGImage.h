//
// Created by dang on 2/12/19.
//

#ifndef APIDEMO_CGIMAGE_H
#define APIDEMO_CGIMAGE_H

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

#include <cairo/cairo.h>

class CGImage; typedef CGImage* CGImageRef;
class CGImage : public cf::Object {
    cairo_surface_t *surface;
    int width = -1;
    int height = -1;
public:
    const char *getTypeName() const override {
        return "CGImage";
    }

    CGImageRef copy() override {
        // immutable, use method (CGImageCreateCopy) to make a copy if you really want
        return retain();
    }

    CGImage(cairo_surface_t *surface, int width, int height);
    ~CGImage() override;
    cairo_surface_t *getSurface(int *width, int *height);

    RETAIN_AND_AUTORELEASE(CGImage);
};


#endif //APIDEMO_CGIMAGE_H
