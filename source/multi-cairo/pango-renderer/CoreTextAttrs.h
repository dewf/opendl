//
// Created by dang on 9/26/18.
//

#ifndef __DL__CORETEXTATTRS_H__
#define __DL__CORETEXTATTRS_H__

#include <cairo.h>
#include <pango/pango.h>
#include "../../common/classes/CF/CFTypes.h"
#include "../classes/CG/CGColor.h"
#include "../classes/CT/CTFont.h"

class CGContext;

class CoreTextAttrs : public PangoAttribute {
    cf::MutableDictionaryRef dict;

    inline cf::ObjectRef getValueFor(cf::StringRef key) {
        cf::ObjectRef value = nullptr;
        if (dict->getValueIfPresent((cf::ObjectRef) key, &value)) {
            return value;
        } else {
            return nullptr;
        }
    }

public:
    bool operator==(const CoreTextAttrs &b) const {
        // we do a simple pointer comparison, no real reason to deep-compare the dictionaries (... probably)
        return dict == b.dict;
    }

    CoreTextAttrs()
        :dict(new cf::MutableDictionary()) {
        // very important!!!
        // should be safe since base is initialized, no?
        pango_attribute_init(this, getCustomFormatClass());
    }

    explicit CoreTextAttrs(cf::DictionaryRef attrs)
        :dict(new cf::MutableDictionary(attrs)) {
        pango_attribute_init(this, getCustomFormatClass());
    }

    CoreTextAttrs(const CoreTextAttrs &other)
            : PangoAttribute(other),
              dict(new cf::MutableDictionary(other.dict))
    {
        pango_attribute_init(this, getCustomFormatClass());
    }

    ~CoreTextAttrs() {
        dict->release();
    }

    CGColorRef getForegroundColor() {
        return (CGColorRef) getValueFor((cf::StringRef) dl_kCTForegroundColorAttributeName);
    }

    CGColorRef getForegroundColorFromContext(CGContext *context);

    CTFontRef getFont() {
        return (CTFontRef) getValueFor((cf::StringRef) dl_kCTFontAttributeName);
    }

    bool getStrokeWidth(float *out) {
        auto valueRef = (cf::NumberRef) getValueFor((cf::StringRef) dl_kCTStrokeWidthAttributeName);
        return valueRef && valueRef->getValue(dl_kCFNumberFloatType, out); // numberref was valid and successfully yielded the requested type
    }

    CGColorRef getStrokeColor() {
        return (CGColorRef) getValueFor((cf::StringRef) dl_kCTStrokeColorAttributeName);
    }

    cf::DictionaryRef getRawAttrs() { return dict; }

    void mergeFrom(CoreTextAttrs *other_attrs) {
        dict->mergeFrom(other_attrs->dict);
    }

    static PangoAttrType pangoAttrType();

private:
    // misc things for Pango Attribute registration
    static PangoAttribute *customFormatCopy(const PangoAttribute *attr);

    static void customFormatDestroy(PangoAttribute *attr);

    static gboolean customFormatEqual(const PangoAttribute *attr1, const PangoAttribute *attr2);

    static PangoAttrClass *getCustomFormatClass();
};

#endif //__DL__CORETEXTATTRS_H__
