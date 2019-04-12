//
// Created by dang on 8/11/18.
//

#ifndef APIDEMO_STRINGATTRMANAGER_H
#define APIDEMO_STRINGATTRMANAGER_H

#include "../../common/AbstractRangeManager.h"
#include "ExtendedAttrs.h"
#include <pango/pango.h>

class StringAttrManager : public AbstractRangeMananger<ExtendedAttrs, ExtendedAttrs> {
private:
    PangoAttrList *attrList = nullptr;
protected:
    ExtendedAttrs *newAccum() override {
        // return a new, blank accumulator to use
        return new ExtendedAttrs();
    };
    void releaseAccum(ExtendedAttrs *m) override {
        // it's being held by the pango layout
        //   so it's not our responsibility to free these
        //   (will crash if we do)
    };
    void accumulate(ExtendedAttrs *accum, ExtendedAttrs *item) override {
        // accumulate item into accum. (foldLeft, essentially)
        accum->mergeFrom(item);
    };
    void applyRangeSpec(RangeSpec *rangeSpec) override {
        // do something with the now non-overlapping range
        auto attr = rangeSpec->spec;
        attr->start_index = (guint)rangeSpec->range.location;
        attr->end_index = (guint)((rangeSpec->range.location + rangeSpec->range.length));
//    printf("insert attr type %d @ %d through %d\n", ExtendedAttrs::pangoAttrType(), attr->start_index, attr->end_index);
//    attr->dumpKeys();
        pango_attr_list_insert(attrList, attr);
    };

public:
    ~StringAttrManager() override
    {
        // delete the effects that have been placed
        for (auto &range : ranges) {
            delete range.spec;
        }
    }

    void setAttr(ExtendedAttrs *attrsSpec, unsigned int start, unsigned int length) {
        // setting start/end really isn't necessary, a whole new ExtendedAttr will be created with a valid range set via applyRangeSpec() callback
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

#endif //APIDEMO_STRINGATTRMANAGER_H
