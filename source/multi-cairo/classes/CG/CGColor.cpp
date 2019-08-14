#include "CGColor.h"

using namespace cf;

CGColorRef CGColor::White = makeEternal(new CGColor(1, 1, 1));
CGColorRef CGColor::Black = makeEternal(new CGColor(0, 0, 0));
CGColorRef CGColor::Clear = makeEternal(new CGColor(0, 0, 0, 0));

size_t CGColor::getNumberOfComponents() {
    return 4; // for now, always 4
}

const dl_CGFloat *CGColor::getComponents() {
    components[0] = red;
    components[1] = green;
    components[2] = blue;
    components[3] = alpha;
    return components;
}

void CGColor::apply(cairo_t *cr) {
    cairo_set_source_rgba(cr, red, green, blue, alpha);
}
