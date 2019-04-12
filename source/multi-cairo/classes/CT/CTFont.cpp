#include "CTFont.h"

#include <fontconfig/fontconfig.h>
#include <pango/pangofc-fontmap.h>

PangoFontMap *CTFont::defaultFontMap = pango_cairo_font_map_get_default();
PangoContext *CTFont::defaultPangoContext = pango_font_map_create_context(defaultFontMap);
CTFontRef CTFont::defaultFont = new CTFont("LiberationSans", 12.0, NULL);
// hmm, this depends on the other two, does it always get initialized last?
static bool initCTFont() {
    pango_cairo_font_map_set_resolution((PangoCairoFontMap *)CTFont::defaultFontMap, 72.0);
    pango_cairo_context_set_resolution(CTFont::defaultPangoContext, 72.0);
}
static bool initalized = initCTFont();

CTFontRef
CTFont::createWithFontDescriptor(CTFontDescriptorRef descriptor, dl_CGFloat size, const dl_CGAffineTransform *matrix) {
    auto config = pango_fc_font_map_get_config((PangoFcFontMap *)defaultFontMap);
    if (FcConfigAppFontAddFile(config, (const FcChar8 *) descriptor->getPath().c_str())) {
        printf("successfully added font file\n");

        // rescan voodoo
        pango_fc_font_map_config_changed((PangoFcFontMap *)defaultFontMap);
        pango_font_map_changed(defaultFontMap);

        return new CTFont(descriptor->getFamilyName(), size, matrix);
    } else {
        return nullptr;
    }
}

