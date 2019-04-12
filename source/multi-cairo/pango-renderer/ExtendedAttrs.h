//
// Created by dang on 8/11/18.
//

#ifndef APIDEMO_EXTENDEDATTRS_H
#define APIDEMO_EXTENDEDATTRS_H

#include <cairo.h>
#include <pango/pango.h>
#include <map>

enum UnderlineStyle {
    None,
    Single,
    Double,
    Triple,
    Squiggly,
    Overline
};

struct RGBAColor {
    float r, g, b, a;

    RGBAColor(float r, float g, float b, float a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    void set_cairo_color(cairo_t *cr) {
        cairo_set_source_rgba(cr, r, g, b, a);
    }
};

struct IntWrapper {
    int value;

    explicit IntWrapper(int value) {
        this->value = value;
    }
};

extern const char *kCustomFormatForeground;
extern const char *kCustomFormatBackground;
extern const char *kCustomFormatHighlight;
extern const char *kCustomFormatStrikeCount;
extern const char *kCustomFormatStrikeColor;
extern const char *kCustomFormatUnderlineStyle;
extern const char *kCustomFormatUnderlineColor;

class ExtendedAttrs : public PangoAttribute {
private:
    std::map<const char *, void *> dict;

    static PangoAttribute *customFormatCopy(const PangoAttribute *attr)
    {
        auto other = *((const ExtendedAttrs *) attr);
        return new ExtendedAttrs(other); // copy constructor
    }
    static void customFormatDestroy(PangoAttribute *attr)
    {
        delete ((ExtendedAttrs *) attr);
    }
    static gboolean customFormatEqual(const PangoAttribute *attr1, const PangoAttribute *attr2)
    {
        return ((ExtendedAttrs *) attr1) == ((ExtendedAttrs *) attr2);
    }

    static PangoAttrClass *getCustomFormatClass(); // defined in the cpp file, since it uses a static variable that there's no need to expose here
public:
    ExtendedAttrs() {
        // very important!!!
        // should be safe since base is initialized, no?
        pango_attribute_init(this, getCustomFormatClass());
    }

    ExtendedAttrs(const ExtendedAttrs &other)
            : _PangoAttribute(other), dict(other.dict)
    {
        // very important!!!
        // should be safe since base is initialized, no?
        pango_attribute_init(this, getCustomFormatClass());
    }

    ~ExtendedAttrs() {
//        printf("ExtendedAttrs freed! (%d - %d)\n", start_index, end_index);
    }

    bool operator==(const ExtendedAttrs &b) const {
        return (dict.size() == b.dict.size()) &&
               std::equal(dict.begin(), dict.end(), b.dict.begin());
    }

    void addFormat(const char *key, void *data) {
        dict[key] = data;
    }

    void *getFormatFor(const char *key) {
        auto found = dict.find(key);
        if (found != dict.end()) {
            return found->second;
        }
        return nullptr;
    }

    static ExtendedAttrs *createFormat(const char *key, void *data) {
        auto ret = new ExtendedAttrs;
        //pango_attribute_init(ret, getCustomFormatClass()); // doing in constructor now
        ret->addFormat(key, data);
        return ret;
    }

    void mergeFrom(ExtendedAttrs *other_attrs);

    void dumpKeys() {
        for (auto &i: dict) {
            printf("  - key %s value %p\n", i.first, i.second);
        }
    }

    static PangoAttrType pangoAttrType();
};


#endif //APIDEMO_EXTENDEDATTRS_H
