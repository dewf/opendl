#include "CGBitmapContext.h"

CGBitmapContextRef CGBitmapContext::copy() {
    throw cf::Exception("CGBitmapContext can't be copied");
}

CGBitmapContext::CGBitmapContext(cairo_surface_t *surface, cairo_t *cr, int width, int height)
    :surface(surface),
     cr(cr),
     width(width), height(height),
     CGContext(cr, width, height)
{
    cairo_surface_reference(surface);
    cairo_reference(cr);
}

CGBitmapContext::~CGBitmapContext()
{
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

CGImageRef CGBitmapContext::createImage() {
    // make copy of surface
    auto data = cairo_image_surface_get_data(surface);
    auto stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
    auto surface2 = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_ARGB32, width, height, stride);

    auto result = new CGImage(surface2, width, height);
    cairo_surface_destroy(surface2); // safe to unref now

    return result;
}

CGBitmapContextRef
CGBitmapContext::create(void *data, size_t width, size_t height, size_t bitsPerComponent, size_t bytesPerRow,
                        CGColorSpaceRef space, dl_CGBitmapInfo bitmapInfo)
{
    cairo_t *cr = nullptr;
    cairo_surface_t *surface = nullptr;

    if (data) {
        throw cf::Exception("CCGBitmapContext::create - we don't yet handle non-null data parameter");
    }

    auto cgSpace = (CGColorSpaceRef)space;
    if (cgSpace->getSpaceType() == CGColorSpace::ColorSpace_DeviceDependent) {
        // even though it's gray (and probably just going to be used for alpha),
        // we want to be able to paint things in grayscale and then later convert / use as true alpha
        assert(bitsPerComponent == 8);
        assert(bytesPerRow == width); // well, internally we're using width * 4, but the client doesn't have to know that
        assert(bitmapInfo == dl_kCGBitmapByteOrderDefault);

        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (int)width, (int)height);
        cr = cairo_create(surface);
        cairo_set_source_rgba(cr, 0, 0, 0, 1);
        cairo_paint(cr);
    } else {
        printf("*** unknown colorspace type in dl_CGBitmapContextCreate\n");
        return nullptr;
    }

    auto result = new CGBitmapContext(surface, cr, (int)width, (int)height);
    cairo_destroy(cr); // safe to unref now
    cairo_surface_destroy(surface); // ditto

    return result;
}
