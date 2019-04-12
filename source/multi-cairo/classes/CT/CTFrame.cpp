//
// Created by dang on 9/25/18.
//

#include "CTFrame.h"
#include "../../pango-renderer/CoreTextAttrs.h"

cf::ArrayRef CTFrame::getLines() {
    // would be nice to have an Array() constructor that takes std::vector<cf::ObjectRef>
    // ... but don't know how to make that covariant (to accept std::vector<whatever>)
    auto ret = new cf::MutableArray();
    for (auto &line: lines) {
        ret->appendValue(line);
    }
    return ret;
}

void CTFrame::getLineOrigins(dl_CFRange range, dl_CGPoint *origins) {
    if (range.length == 0) {
        range.location = 0;
        range.length = lines.size();
    } else {
        assert(range.length > 0
         && range.location >= 0
         && (range.location + range.length) <= lines.size());
    }
    for (dl_CFIndex i = range.location; i < (range.location + range.length); i++) {
        auto lineOrigin = lines[i]->getOrigin();
        auto x = rect.origin.x + lineOrigin.x;
        auto y = rect.origin.y + lineOrigin.y;
        *origins++ = dl_CGPointMake(x, y); //lines[i]->getOrigin();
    }
}

const char *CTFrame::getTypeName() const {
    return "CTFrame";
}

CTFrameRef CTFrame::copy() {
    return retain();
}
