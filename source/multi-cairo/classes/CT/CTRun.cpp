//
// Created by dang on 9/27/18.
//

#include "CTRun.h"

bool rangeEq(const dl_CFRange &a, const dl_CFRange &b) {
    return (a.location == b.location && a.length == b.length);
}

double CTRun::getTypographicBounds(dl_CFRange range, dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *leading) const {
    if (!rangeEq(range, dl_CFRangeZero)) {
        throw cf::Exception("CTRun::getTypographicBounds doesn't yet support non-zero ranges on linux");
    }
    if (ascent) {
        //*ascent = pango_units_to_double(pango_font_metrics_get_ascent(run.metrics));
        *ascent = run.baseLine;
    }
    if (descent) {
        //*descent = pango_units_to_double(pango_font_metrics_get_descent(run.metrics));
        *descent = run.logicalRect.size.height - run.baseLine;
    }
    if (leading) {
        *leading = 0; // until we have some better information ...
    }
    return run.logicalRect.size.width;
}

