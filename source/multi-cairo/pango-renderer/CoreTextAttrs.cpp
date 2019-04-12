//
// Created by dang on 9/26/18.
//

#include "CoreTextAttrs.h"
#include "../classes/CG/CGContext.h"

#define ATTR_TYPE_NAME "CoreTextAttrType"

PangoAttribute *CoreTextAttrs::customFormatCopy(const PangoAttribute *attr) {
    auto other = ((const CoreTextAttrs *) attr);
    return new CoreTextAttrs(*other); // copy constructor
}

void CoreTextAttrs::customFormatDestroy(PangoAttribute *attr) {
    delete ((CoreTextAttrs *) attr);
}

gboolean CoreTextAttrs::customFormatEqual(const PangoAttribute *attr1, const PangoAttribute *attr2) {
    return *((CoreTextAttrs *) attr1) == *((CoreTextAttrs *) attr2);
}

static PangoAttrClass *customFormatClass = nullptr;

PangoAttrClass *CoreTextAttrs::getCustomFormatClass() {
    if (!customFormatClass) {
        customFormatClass = new PangoAttrClass;
        customFormatClass->type = pango_attr_type_register(ATTR_TYPE_NAME);
        customFormatClass->copy = customFormatCopy;
        customFormatClass->destroy = customFormatDestroy;
        customFormatClass->equal = customFormatEqual;
    }
    return customFormatClass;
}

PangoAttrType CoreTextAttrs::pangoAttrType() {
    return customFormatClass->type;
}

CGColorRef CoreTextAttrs::getForegroundColorFromContext(CGContext *context) {
    auto bval = (cf::BooleanRef)getValueFor((cf::StringRef)dl_kCTForegroundColorFromContextAttributeName);
    if (bval && bval->getValue()) {
        return context->getFillColor();
    } else {
        return nullptr;
    }
}
