#include <cmath>

#ifndef __CT_LINE_H__
#define __CT_LINE_H__

#include <pango/pangocairo.h>
#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

#include "../CG/CGContext.h"
#include "CTFont.h"
#include "CTRun.h"

#include "../../pango-renderer/MyPangoLayout.h"

#include <limits>

class CTLine; typedef CTLine *CTLineRef;
class CTLine : public cf::Object {
    PangoLineStruct lineStruct;
    MyPangoLayoutRef pangoLayout = nullptr; // might be a weak reference
    bool layoutOwned = false;
public:
    const char *getTypeName() const override {
        return "CTLine";
    }

    CTLineRef copy() override {
        // immutable
        return retain();
    }

    CTLine(PangoLineStruct &lineStruct, MyPangoLayoutRef pangoLayout, bool layoutOwned = true)
        :lineStruct(lineStruct), layoutOwned(layoutOwned)
    {
        if (layoutOwned) {
            this->pangoLayout = pangoLayout->retain();
        } else {
            this->pangoLayout = pangoLayout;
        }
    }

    static CTLineRef createWithAttributedString(cf::AttributedStringRef attrString) {
        auto layout = new MyPangoLayout(attrString);
        auto ls = layout->layoutSingleLine();
        auto line = new CTLine(ls, layout);
        layout->release(); // line now owns reference to layout
        return line;
    }

    ~CTLine() override {
        if (pangoLayout && layoutOwned) {
            pangoLayout->release();
        }
    }

    dl_CGPoint getOrigin() const {
//        return dl_CGPointMake(lineStruct.logicalRect.origin.x, /*lineStruct.logicalRect.origin.y +*/ lineStruct.baseline);
        return lineStruct.origin;
    }

    dl_CGRect getBoundsWithOptions(dl_CTLineBoundsOptions options) const {
//        dl_kCTLineBoundsExcludeTypographicLeading  = 1 << 0,
//        dl_kCTLineBoundsExcludeTypographicShifts   = 1 << 1,
//        dl_kCTLineBoundsUseHangingPunctuation      = 1 << 2,
//        dl_kCTLineBoundsUseGlyphPathBounds         = 1 << 3,
//        dl_kCTLineBoundsUseOpticalBounds           = 1 << 4
        // these need to be relative to the baseline starting point
        if (options & dl_kCTLineBoundsUseGlyphPathBounds) {
            auto ret = lineStruct.inkRect;
            //ret.origin.y -= lineStruct.baseline;
            return ret;
        } else {
            auto ret = lineStruct.logicalRect;
            return ret;
//            if (pangoLayout->rightToLeft) {
//                auto origin = getOrigin();
//            } else {
//                return ret;
//            }
        }
    }

    double getTypographicBounds(dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *leading) const {
        if (ascent) {
            *ascent = lineStruct.baseline;
        }
        if (descent) {
            *descent = lineStruct.logicalRect.size.height - lineStruct.baseline;
        }
        if (leading) {
            *leading = pangoLayout->getSpacing();
        }
        // width
        return lineStruct.logicalRect.size.width;
    }

    void draw(CGContextRef context) const {
        auto savedMatrix = context->getMatrix(); // can't save/restore gstate because that affects clipping

        dl_CGFloat yExtra = 0;
        if (layoutOwned) {
            // we're a standalone line that owns our own layout,
            //  so can't rely on a surrounding frame (with a defined path/rect)
            //  to put us in the right place
            context->translateToTextPosition();
        }
//        else {
//            yExtra += lineStruct.baseline; // need a little push to make it to the origin, from the top (corner) of the line logical rect
//        }

        // can't use the built-in layout draw because we handle foreground colors a bit differently
        //  (pango has no knowledge of our drawing context or its foreground color)
        auto cr = context->getCairoContext();

        for (auto &run: lineStruct.runs) {
            auto x = run.logicalRect.origin.x;
            //auto y = lineStruct.logicalRect.origin.y + yExtra; // needs to be: line origin for unowned line, line origin + baseline for framed lines
            auto y = layoutOwned ? 0 : lineStruct.origin.y;

            context->drawGlyphRun(run, x, y);
        }

        if (layoutOwned) {
            // standalone line needs to advance text position
            // (maybe other lines too? not sure yet)
            context->advanceTextPosition(lineStruct.logicalRect.size.width);
        }

        context->setMatrix(savedMatrix); // see note at beginning of method re: gstate
    }

    cf::ArrayRef getGlyphRuns() const {
        auto ret = new cf::MutableArray();
        for (auto &r : lineStruct.runs) {
            auto run = new CTRun(r);
            ret->appendValue(run);
            run->release();
        }
        return ret;
    }

    dl_CGFloat getOffsetForStringIndex(dl_CFIndex charIndex, dl_CGFloat* secondaryOffset) const;
//    {
//        // get the X offset for that character index
//        // this means knowing the advances for each of the glyphs/runs in this lineStruct
//        auto lineAdjusted = charIndex - lineStruct.stringRange.location;
//        if (lineAdjusted >= 0 && lineAdjusted < lineStruct.stringRange.length) {
//            // iterate over all the glyphs of all the runs
//            dl_CGFloat offset = 0;
//            for (auto &r : lineStruct.runs) {
//                auto runAdjusted = charIndex - r.stringRange.location;
//                if (runAdjusted >= 0 && runAdjusted < r.stringRange.length) {
//                    // get the offset at index [runAdjusted]
//                    auto result = r.offsets[runAdjusted].xOffset;
//                    if (secondaryOffset) {
//                        *secondaryOffset = result; // not using this properly yet
//                    }
//                    return result;
//                }
//            }
//        } else {
//            throw cf::Exception("CTLine::getOffsetForStringIndex() - charIndex out of range");
//        }
//    }

    dl_CFIndex getStringIndexForPosition(dl_CGPoint position) const;

    dl_CFRange getStringRange() const;

    RETAIN_AND_AUTORELEASE(CTLine);
    WEAKREF_MAKE(CTLine);
};

#endif //__CT_LINE_H__
