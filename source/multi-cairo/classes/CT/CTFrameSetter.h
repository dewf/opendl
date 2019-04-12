//
// Created by dang on 9/25/18.
//

#ifndef __CT_FRAMESETTER_H__
#define __CT_FRAMESETTER_H__

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

#include "../CG/CGPath.h"
#include "CTFrame.h"

class CTFrameSetter;

typedef CTFrameSetter *CTFrameSetterRef;

class CTFrameSetter : public cf::Object {
    cf::AttributedStringRef attrString;
public:
    const char *getTypeName() const override {
        return "CTFrameSetter";
    }

    CTFrameSetterRef copy() override {
        // immutable
        return (CTFrameSetterRef) retain();
    }

    // public API methods
    CTFrameSetter(cf::AttributedStringRef attrString)
            : attrString(attrString->copy()) {
    }
    ~CTFrameSetter() {
        attrString->release();
    }

    CTFrameRef createFrame(dl_CFRange stringRange, CGPathRef path, cf::DictionaryRef frameAttrs) {
        cf::AttributedStringRef sub;
        if (stringRange.length == 0) {
            sub = attrString; // full string
        }
        else {
            sub = cf::AttributedString::createWithSubstring(attrString, stringRange);
        }
        return new CTFrame(sub, path, frameAttrs);
    }
};

#endif //APIDEMO_CTFRAMESETTER_H
