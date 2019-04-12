//
// Created by dang on 8/10/18.
//

#include "CustomPangoRenderer.h"
#include <pango/pangocairo.h>

typedef struct {
    double x, y;
} Point;

/* util funcs */
static void to_device (PangoMatrix *matrix,
                       double       x,
                       double       y,
                       Point       *result)
{
    if (matrix)
    {
        result->x = (x * matrix->xx + y * matrix->xy) / PANGO_SCALE + matrix->x0;
        result->y = (x * matrix->yx + y * matrix->yy) / PANGO_SCALE + matrix->y0;
    }
    else
    {
        result->x = x / PANGO_SCALE;
        result->y = y / PANGO_SCALE;
    }
}


/* class methods */

CustomPangoRenderer::CustomPangoRenderer(cairo_t *context) {
    this->cr = context;
    fg_color = nullptr;
    bg_color = nullptr;
}

CustomPangoRenderer::~CustomPangoRenderer() {
}

void CustomPangoRenderer::draw_glyphs(PangoFont *font, PangoGlyphString *glyphs, int x, int y) {
    Point p;
    to_device(nullptr, x, y, &p);
    cairo_move_to(cr, p.x, p.y);

    if (bg_color) {
//        pango_layout_exte
    }

    if (fg_color) {
        cairo_set_source_rgb(cr, fg_color->red/65535.0, fg_color->green/65535.0, fg_color->blue/65535.0);
    } else {
        cairo_set_source_rgb(cr, 0, 0, 0);
    }
    pango_cairo_show_glyph_string(cr, font, glyphs);
}

void CustomPangoRenderer::draw_glyph_item(const char *text, PangoGlyphItem *glyph_item, int x, int y) {
    printf("draw_item\n");
}

void CustomPangoRenderer::draw_rectangle(PangoRenderPart part, int x, int y, int width, int height) {
    printf("draw_rectangle\n");
}

void CustomPangoRenderer::draw_error_underline(int x, int y, int width, int height) {
    printf("draw_underline\n");
}

void CustomPangoRenderer::prepare_run(PangoLayoutRun *run) {
    // basically examine the attributes and get prepared to render them in the next call
    fg_color = nullptr;
    bg_color = nullptr;
    for (auto item = run->item->analysis.extra_attrs; item != NULL; item = item->next) {
        auto attr = (PangoAttribute *)item->data;
        switch (attr->klass->type) {
            case PANGO_ATTR_FOREGROUND:
                fg_color = &((PangoAttrColor *)attr)->color;
                break;
            case PANGO_ATTR_BACKGROUND:
                bg_color = &((PangoAttrColor *)attr)->color;
                break;
            default:
                // nothing yet
                break;
        }
    }
}


