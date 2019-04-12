//
// Created by dang on 2/12/19.
//

#include "CGImage.h"

CGImage::CGImage(cairo_surface_t *surface, int width, int height)
    :surface(surface), width(width), height(height)
{
    cairo_surface_reference(surface);
}

CGImage::~CGImage() {
    cairo_surface_destroy(surface);
}

cairo_surface_t *CGImage::getSurface(int *width, int *height) {
    if (width) {
        *width = this->width;
    }
    if (height) {
        *height = this->height;
    }
    return surface;
}
