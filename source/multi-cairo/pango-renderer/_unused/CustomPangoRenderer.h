//
// Created by dang on 8/10/18.
//

#ifndef APIDEMO_CUSTOMPANGORENDERER_H
#define APIDEMO_CUSTOMPANGORENDERER_H

#include <cairo.h>
#include <pango/pango.h>

class CustomPangoRenderer {
private:
    cairo_t *cr;
    PangoColor *fg_color, *bg_color;
public:
    explicit CustomPangoRenderer(cairo_t *context);
    ~CustomPangoRenderer(); // from finalize()
    void draw_glyphs(PangoFont *font, PangoGlyphString *glyphs, int x, int y);
    void draw_glyph_item(const char *text, PangoGlyphItem *glyph_item, int x, int y);
    void draw_rectangle(PangoRenderPart part, int x, int y, int width, int height);
    void draw_error_underline(int x, int y, int width, int height);
    void prepare_run(PangoLayoutRun *run);
};


#endif //APIDEMO_CUSTOMPANGORENDERER_H
