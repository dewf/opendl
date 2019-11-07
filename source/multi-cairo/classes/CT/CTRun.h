#include <utility>

//
// Created by dang on 9/27/18.
//

#ifndef __CT_RUN_H__
#define __CT_RUN_H__

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"

#include "../../pango-renderer/MyPangoLayout.h"

class CTRun; typedef CTRun *CTRunRef;
class CTRun : public cf::Object {
    const PangoRunStruct run;
public:
    const char *getTypeName() const override {
        return "CTRun";
    }

    CTRunRef copy() override {
        return retain();
    }

    explicit CTRun(PangoRunStruct run)
            : run(std::move(run)) // ??
    {
        // nothing yet
    }

    cf::DictionaryRef getAttributes() const {
        return run.attrs->getRawAttrs();
    }

    double getTypographicBounds(dl_CFRange range, dl_CGFloat* ascent, dl_CGFloat* descent, dl_CGFloat* leading) const;

    dl_CFRange getStringRange() const {
        return run.stringRange;
    }

    dl_CTRunStatus getRunStatus() const {
        return run.runStatus;
    }

    RETAIN_AND_AUTORELEASE(CTRun);
};

#endif //__CT_RUN_H__
