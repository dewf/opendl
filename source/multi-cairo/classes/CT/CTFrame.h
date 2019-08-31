#ifndef __CT_FRAME_H__
#define __CT_FRAME_H__

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

#include "../CG/CGContext.h"
#include "../CG/CGPath.h"
//#include "../CT/CTFont.h"

#include "../../pango-renderer/MyPangoLayout.h"
#include "CTLine.h"

#include <vector>

class CTFrame; typedef CTFrame *CTFrameRef;
class CTFrame : public cf::Object {
    MyPangoLayoutRef pangoLayout = nullptr;
    std::vector<CTLineRef> lines;
    cf::ArrayRef publicLines = nullptr; // CFArray around the lines, for public reading
    CGPathRef path = nullptr;
    dl_CGRect rect = dl_CGRectZero;
public:
    const char *getTypeName() const override;
    CTFrameRef copy() override;

    CTFrame(cf::AttributedStringRef attrString, CGPathRef path, cf::DictionaryRef frameAttrs)
        :path(path->copy())
    {
        pangoLayout = new MyPangoLayout(attrString); // pass frameAttrs on to this?
        rect = path->getRect(); // throws if not a rect
        auto lineStructs = pangoLayout->layoutArea(rect.size.width, rect.size.height);
        for (auto &ls: lineStructs) {
            lines.push_back(new CTLine(ls, pangoLayout, false)); // false = not owned / weak ref to layout
        }
        publicLines = cf::Array::create((cf::ObjectRef *)lines.data(), lines.size());
    }

    ~CTFrame() override {
        publicLines->release();
        for (auto line: lines) {
            line->release();
        }
        pangoLayout->release();
        path->release();
    }

    void draw(CGContextRef context) {
        context->saveGState();

        // if we ever wanted to clip instead of ellipsize at height ...
        // context->clipToRect(path->getRect());

        context->translateCTM(rect.origin.x, rect.origin.y);

        // can't use the built-in layout draw because we handle foreground colors a bit differently
        //  (pango has no knowledge of our drawing context or its foreground color)
        // now perhaps we could do something similar to the directwrite situation
        //  (where we have a custom object that receives callbacks for every glyph run,
        //   allowing us to consult our graphics context) - and maybe that's a possibility
        //   with a custom pango renderer, but we didn't end up going that route thus far
        for (auto &line: lines) {
            line->draw(context);
        }

        context->restoreGState();
    }

    cf::ArrayRef getLines() {
        return publicLines;
    };
    void getLineOrigins(dl_CFRange range, dl_CGPoint origins[]);

    RETAIN_AND_AUTORELEASE(CTFrame);
    WEAKREF_MAKE(CTFrame);
};


#endif //__CT_FRAME_H__
