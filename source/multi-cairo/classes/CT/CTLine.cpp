//
// Created by dang on 9/23/18.
//

#include "CTLine.h"

dl_CGFloat CTLine::getOffsetForStringIndex(dl_CFIndex charIndex, dl_CGFloat *secondaryOffset) const
{
    // perhaps do some checking to verify charIndex is in line char range, and pango function returns our line number
    auto _layout = pangoLayout->getRawLayout();
    int lineIndex, xpos;
    pango_layout_index_to_line_x(_layout, (int)charIndex, false, &lineIndex, &xpos);
    auto result = pango_units_to_double(xpos);

    auto stringRange = getStringRange();
    if (pangoLayout->rightToLeft && charIndex == dl_CFRangeEnd(stringRange) - 1) {
        // special case for RTL layout, on newline
        // otherwise it wants to put the offset at the beginning (right side) of the line (?)
        result = 0;
    }

    if (secondaryOffset) {
        *secondaryOffset = result;
    }
    return result;
}

dl_CFIndex CTLine::getStringIndexForPosition(dl_CGPoint position) const
{
    auto _layout = pangoLayout->getRawLayout();
    // incoming position is relative to line origin
    // but pango wants the 'absolute' position (relative to frame or whatever)
    auto origin = getOrigin();
    position.x += origin.x;
    position.y += origin.y;
    auto x = pango_units_from_double(position.x);
    auto y = pango_units_from_double(position.y);
    int index, trailing;
    if (pango_layout_xy_to_index(_layout, x, y, &index, &trailing)) {
        return index + trailing;
    } else {
        printf("notfound\n");
        return dl_kCFNotFound;
    }
}

dl_CFRange CTLine::getStringRange() const {
    auto line = lineStruct.line;
    auto endOfLineIndex = line->start_index + line->length;
    // trying to match the behavior of mac/win - pango tries to skip over newlines, making it hard to place the cursor
    // see also the special RTL handling in getOffsetForStringIndex()
    auto extra = pangoLayout->isNewlineAtIndex(endOfLineIndex) ? 1 : 0;
    return dl_CFRangeMake(lineStruct.line->start_index, lineStruct.line->length + extra);
}

