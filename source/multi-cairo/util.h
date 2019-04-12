//
// Created by dang on 9/23/18.
//

#ifndef __DL_UTIL_H__
#define __DL_UTIL_H__

#include <pango/pangocairo.h>
#include "../opendl.h"

#include <vector>

inline cairo_matrix_t dl_to_cairo_matrix(dl_CGAffineTransform transform) {
    cairo_matrix_t m;
    cairo_matrix_init(&m, transform.a, transform.b, transform.c, transform.d, transform.tx, transform.ty);
    return m;
}

inline dl_CGAffineTransform cairo_to_dl_matrix(cairo_matrix_t m) {
    dl_CGAffineTransform ret;
    ret.a = m.xx;
    ret.b = m.yx;
    ret.c = m.xy;
    ret.d = m.yy;
    ret.tx = m.x0;
    ret.ty = m.y0;
    return ret;
}

#endif //__DL_UTIL_H__
