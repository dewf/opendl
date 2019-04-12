//
// Created by dang on 2/9/19.
//

#ifndef APIDEMO_CORETEXTATTRMANAGER_H
#define APIDEMO_CORETEXTATTRMANAGER_H

#include "../../common/AbstractRangeManager.h"
#include "CoreTextAttrs.h"
#include <pango/pango.h>

class CoreTextAttrManager : public AbstractRangeMananger<CoreTextAttrs, CoreTextAttrs> {
private:
    PangoAttrList *attrList = nullptr;
protected:
    CoreTextAttrs *newAccum() override {
        // return a new, blank accumulator to use
        return new CoreTextAttrs();
    }

    void releaseAccum(CoreTextAttrs *m) override {
        // it's being held by the pango layout
        //   so it's not our responsibility to free these
        //   (will crash if we do)
    }

    void accumulate(CoreTextAttrs *accum, CoreTextAttrs *item) override {
        // accumulate item into accum. (foldLeft, essentially)
        accum->mergeFrom(item);
    }

    void applyRangeSpec(RangeSpec *rangeSpec) override {
        // do something with the now non-overlapping range
        auto attr = rangeSpec->spec;
        attr->start_index = (guint)rangeSpec->range.location;
        attr->end_index = (guint)((rangeSpec->range.location + rangeSpec->range.length));
//    printf("insert attr type %d @ %d through %d\n", ExtendedAttrs::pangoAttrType(), attr->start_index, attr->end_index);
//    attr->dumpKeys();
        pango_attr_list_insert(attrList, attr);
    }

public:
    ~CoreTextAttrManager() {
        // delete the effects that have been placed
        for (auto &range : ranges) {
            delete range.spec;
        }
    }

    void setAttr(CoreTextAttrs *attrsSpec, unsigned int start, unsigned int length) {
        // setting start/end really isn't necessary, a whole new CoreTextAttrs will be created with a valid range set via applyRangeSpec() callback
        // however it makes for nicer printing/debugging anyplace we're wanting to look at that
        // ... maybe
        attrsSpec->start_index = start;
        attrsSpec->end_index = (start + length);
        addRange(dl_CFRangeMake(start, length), attrsSpec);
    }

    void apply(PangoAttrList *attrList) {
        this->attrList = attrList;
        AbstractRangeMananger::apply();
        this->attrList = nullptr;
    }
};


#endif //APIDEMO_CORETEXTATTRMANAGER_H
