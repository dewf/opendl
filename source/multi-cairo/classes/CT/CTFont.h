//
// Created by dang on 9/23/18.
//

#ifndef __CT_FONT_H__
#define __CT_FONT_H__

#include "../../../../deps/CFMinimal/source/CF/CFObject.h"
#include "../../../opendl.h"
#include "../../util.h"
#include <string>
#include <cmath>

#include "CTFontDescriptor.h"

#include <pango/pango.h>

class CTFont;

typedef CTFont *CTFontRef;

class CTFont : public cf::Object {
    std::string name;
    PangoFontDescription *fontDescription = nullptr;
    PangoFont *font = nullptr;
    PangoFontMetrics *metrics = nullptr;
    cairo_matrix_t matrix;

public:
    static CTFontRef defaultFont;
    static PangoFontMap *defaultFontMap; // = pango_cairo_font_map_get_default();
    static PangoContext *defaultPangoContext; // = pango_font_map_create_context(fontMap);

    const char *getTypeName() const override {
        return "CTFont";
    }

    std::string toString() const override {
        return sprintfToStdString("CTFont(%s) [PangoFont %zX] [%s]", name.c_str(), (size_t)font, Object::toString().c_str());
    }

    CTFontRef copy() override {
        // immutable
        return (CTFontRef) retain();
    }

    PangoFontDescription *getDescription() {
        return fontDescription;
    }

    // public API methods ===================================

    CTFont(std::string name, dl_CGFloat size, const dl_CGAffineTransform *matrix)
            : name(name) {

//        auto fmres = pango_cairo_font_map_get_resolution((PangoCairoFontMap *)defaultFontMap);
//        printf("####CTFont::fmres: %.2f\n", fmres);

        char descBuffer[1024];
        snprintf(descBuffer, 1024, "%s %.2f", name.c_str(), size);
        fontDescription = pango_font_description_from_string(descBuffer);

        font = pango_font_map_load_font(defaultFontMap, defaultPangoContext, fontDescription);
        //auto lang = pango_script_get_sample_language(PANGO_SCRIPT_LATIN);
        metrics = pango_font_get_metrics(font, pango_language_get_default());

//                    auto ul_thick_x = pango_units_to_double(pango_font_metrics_get_underline_thickness(metrics));
//                    printf("CTOR///ul_thick_x: %.2f (pango font %zX)\n", ul_thick_x, (size_t)font);
//                    auto desc = pango_font_describe(font);
//                    auto famalam = pango_font_description_get_family(desc);
//                    printf("  = family: %s [%s]\n", famalam, pango_font_description_to_filename(desc));

        if (matrix) {
            this->matrix = dl_to_cairo_matrix(*matrix);
        } else {
            cairo_matrix_init_identity(&this->matrix);
        }
    }

    CTFont(CTFontRef copyFrom, dl_CGFloat size, const dl_CGAffineTransform *matrix, dl_CTFontSymbolicTraits symTraitValue, dl_CTFontSymbolicTraits symTraitMask) {
        fontDescription = pango_font_describe(copyFrom->font);

        if (size != 0) {
            pango_font_description_set_size(fontDescription, (gint)size);
        }
        if (symTraitMask & dl_kCTFontTraitItalic) {
            auto value = (symTraitValue & dl_kCTFontTraitItalic) ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL;
            pango_font_description_set_style(fontDescription, value);
        }

        if (symTraitMask & dl_kCTFontTraitBold) {
            auto value = (symTraitValue & dl_kCTFontTraitBold) ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL;
            pango_font_description_set_weight(fontDescription, value);
        }

        if (symTraitMask & dl_kCTFontTraitExpanded) {
            auto value = (symTraitValue & dl_kCTFontTraitExpanded) ? PANGO_STRETCH_EXPANDED : PANGO_STRETCH_NORMAL;
            pango_font_description_set_stretch(fontDescription, value);
        } else if (symTraitMask & dl_kCTFontTraitCondensed) {
            auto value = (symTraitValue & dl_kCTFontTraitCondensed) ? PANGO_STRETCH_CONDENSED : PANGO_STRETCH_NORMAL;
            pango_font_description_set_stretch(fontDescription, value);
        }

        char nameBuffer[1024];
        snprintf(nameBuffer, 1024, "(modified copy of [%s])", copyFrom->name.c_str());
        name = nameBuffer;

        font = pango_font_map_load_font(defaultFontMap, defaultPangoContext, fontDescription);
        metrics = pango_font_get_metrics(font, pango_language_get_default());

        if (matrix) {
            this->matrix = dl_to_cairo_matrix(*matrix);
        } else {
            this->matrix = copyFrom->matrix;
        }
    }

    ~CTFont() {
        pango_font_metrics_unref(metrics);
        g_object_unref(font); // ??
        pango_font_description_free(fontDescription);
    }

    static CTFontRef createCopyWithSymbolicTraits(CTFontRef copyFrom, dl_CGFloat size, const dl_CGAffineTransform *matrix, dl_CTFontSymbolicTraits symTraitValue, dl_CTFontSymbolicTraits symTraitMask) {
        return new CTFont(copyFrom, size, matrix, symTraitValue, symTraitMask);
    }

    static CTFontRef createWithFontDescriptor(CTFontDescriptorRef descriptor, dl_CGFloat size, const dl_CGAffineTransform *matrix);

    dl_CGFloat getUnderlineThickness() {
        return pango_units_to_double(pango_font_metrics_get_underline_thickness(metrics));
    }

    dl_CGFloat getUnderlinePosition() {
        return -1 * pango_units_to_double(pango_font_metrics_get_underline_position(metrics));
    }
};

#endif //__CT_FONT_H__
