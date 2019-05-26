//
// Created by dang on 2/12/19.
//

#ifndef APIDEMO_CGBITMAPCONTEXT_H
#define APIDEMO_CGBITMAPCONTEXT_H

#include "CGContext.h"
#include "CGImage.h"

#include "CGColorSpace.h"

#include <cairo/cairo.h>

class CGBitmapContext; typedef CGBitmapContext* CGBitmapContextRef;
class CGBitmapContext : public CGContext {
    cairo_surface_t *surface = nullptr;
    cairo_t *cr = nullptr;
    int width = -1;
    int height = -1;
    CGColorSpaceRef colorSpace = nullptr;

    CGBitmapContext(cairo_surface_t *surface, cairo_t *cr, int width, int height, CGColorSpaceRef colorSpace);
public:
    CGBitmapContextRef copy() override;

    ~CGBitmapContext() override;

    CGImageRef createImage();

    static CGBitmapContextRef create(void *data, size_t width,
                               size_t height, size_t bitsPerComponent, size_t bytesPerRow,
                               CGColorSpaceRef space, dl_CGBitmapInfo bitmapInfo);
    void *getData();
    void releaseData();

    RETAIN_AND_AUTORELEASE(CGBitmapContext);
};


#endif //APIDEMO_CGBITMAPCONTEXT_H
