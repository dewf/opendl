//
// Created by dang on 8/11/18.
//

#include "ExtendedAttrs.h"

const char *kCustomFormatForeground = "kCustomFormatForeground"; // RGBAColor *
const char *kCustomFormatBackground = "kCustomFormatBackground"; // RGBAColor *
const char *kCustomFormatHighlight = "kCustomFormatHighlight";   // RGBAColor *
const char *kCustomFormatStrikeCount = "kCustomFormatStrikeCount"; // IntWrapper *
const char *kCustomFormatStrikeColor = "kCustomFormatStrikeColor"; // RGBAColor *
const char *kCustomFormatUnderlineStyle = "kCustomFormatUnderlineStyle"; // IntWrapper * (UnderlineStyle enum)
const char *kCustomFormatUnderlineColor = "kCustomFormatUnderlineColor"; // RGBAColor *

static PangoAttrClass *customFormatClass = nullptr; // needs to be manually initialized

PangoAttrClass *ExtendedAttrs::getCustomFormatClass() {
    if (!customFormatClass) {
        customFormatClass = new PangoAttrClass;
        customFormatClass->type = pango_attr_type_register(
                "CustomFormatClassType"); // using a literal string because this ultimately isn't something we'll be using again ... right?
        customFormatClass->copy = customFormatCopy;
        customFormatClass->destroy = customFormatDestroy;
        customFormatClass->equal = customFormatEqual;
    }

    return customFormatClass;
}

PangoAttrType ExtendedAttrs::pangoAttrType() {
    return customFormatClass->type;
}

void ExtendedAttrs::mergeFrom(ExtendedAttrs *other_attrs) {
    for (auto &it : other_attrs->dict) {
        // we don't own the values in the dict, so no need to free here
        dict[it.first] = it.second;
    }
}
