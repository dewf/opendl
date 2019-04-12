//
// Created by dang on 8/10/18.
//

#include "tf-pango-renderer.h"

struct _tfPangoRendererPrivate {
    CustomPangoRenderer *cpr;
};

static gpointer tf_pango_renderer_parent_class = NULL;

GType tf_pango_renderer_get_type(void) G_GNUC_CONST;

#define TF_PANGO_RENDERER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TF_TYPE_PANGO_RENDERER, tfPangoRendererPrivate))
enum {
    TF_PANGO_RENDERER_DUMMY_PROPERTY
};

tfPangoRenderer *tf_pango_renderer_new(cairo_t *cr);

tfPangoRenderer *tf_pango_renderer_construct(GType object_type, cairo_t *cr);

static void
tf_pango_renderer_real_draw_glyphs(PangoRenderer *base, PangoFont *font, PangoGlyphString *glyphs, gint x, gint y);

static void
tf_pango_renderer_real_draw_glyph_item(PangoRenderer *base, const gchar *text, PangoGlyphItem *glyph_item, gint x,
                                       gint y);

static void tf_pango_renderer_real_draw_rectangle(PangoRenderer *base, PangoRenderPart part, gint x, gint y, gint width,
                                                  gint height);

static void tf_pango_renderer_real_draw_error_underline(PangoRenderer *base, gint x, gint y, gint width, gint height);

static void tf_pango_renderer_real_prepare_run(PangoRenderer *base, PangoLayoutRun *run);

static void tf_pango_renderer_finalize(GObject *obj);

static gpointer _cairo_reference0(gpointer self) {
    return self ? cairo_reference((cairo_t *) self) : NULL;
}

tfPangoRenderer *tf_pango_renderer_construct(GType object_type, CustomPangoRenderer *cpr) {
    tfPangoRenderer *self = NULL;
    g_return_val_if_fail (cpr != NULL, NULL);
    self = (tfPangoRenderer *) g_object_new(object_type, NULL);
    self->priv->cpr = cpr;
    return self;
}

tfPangoRenderer *tf_pango_renderer_new(CustomPangoRenderer *cpr) {
    return tf_pango_renderer_construct(TF_TYPE_PANGO_RENDERER, cpr);
}


static void
tf_pango_renderer_real_draw_glyphs(PangoRenderer *base, PangoFont *font, PangoGlyphString *glyphs, gint x, gint y) {
    tfPangoRenderer *self;
    self = (tfPangoRenderer *) base;
    g_return_if_fail (font != NULL);
    g_return_if_fail (glyphs != NULL);
    self->priv->cpr->draw_glyphs(font, glyphs, x, y);
}

static void
tf_pango_renderer_real_draw_glyph_item(PangoRenderer *base, const gchar *text, PangoGlyphItem *glyph_item, gint x,
                                       gint y) {
    tfPangoRenderer *self;
    self = (tfPangoRenderer *) base;
    g_return_if_fail (glyph_item != NULL);
    self->priv->cpr->draw_glyph_item(text, glyph_item, x, y);
}


static void tf_pango_renderer_real_draw_rectangle(PangoRenderer *base, PangoRenderPart part, gint x, gint y, gint width,
                                                  gint height) {
    tfPangoRenderer *self;
    self = (tfPangoRenderer *) base;
    self->priv->cpr->draw_rectangle(part, x, y, width, height);
}

static void tf_pango_renderer_real_draw_error_underline(PangoRenderer *base, gint x, gint y, gint width, gint height) {
    tfPangoRenderer *self;
    self = (tfPangoRenderer *) base;
    self->priv->cpr->draw_error_underline(x, y, width, height);
}

static void tf_pango_renderer_real_prepare_run(PangoRenderer *base, PangoLayoutRun *run) {
    tfPangoRenderer *self;
    self = (tfPangoRenderer *) base;
    g_return_if_fail (run != NULL);
    self->priv->cpr->prepare_run(run);
}


static void tf_pango_renderer_class_init(tfPangoRendererClass *klass) {
    tf_pango_renderer_parent_class = g_type_class_peek_parent(klass);
    g_type_class_add_private(klass, sizeof(tfPangoRendererPrivate));
    ((PangoRendererClass *) klass)->draw_glyphs = (void (*)(PangoRenderer *, PangoFont *, PangoGlyphString *, gint,
                                                            gint)) tf_pango_renderer_real_draw_glyphs;
    ((PangoRendererClass *) klass)->draw_glyph_item = (void (*)(PangoRenderer *, const gchar *, PangoGlyphItem *, gint,
                                                                gint)) tf_pango_renderer_real_draw_glyph_item;
    ((PangoRendererClass *) klass)->draw_rectangle = (void (*)(PangoRenderer *, PangoRenderPart, gint, gint, gint,
                                                               gint)) tf_pango_renderer_real_draw_rectangle;
    ((PangoRendererClass *) klass)->draw_error_underline = (void (*)(PangoRenderer *, gint, gint, gint,
                                                                     gint)) tf_pango_renderer_real_draw_error_underline;
    ((PangoRendererClass *) klass)->prepare_run = (void (*)(PangoRenderer *,
                                                            PangoLayoutRun *)) tf_pango_renderer_real_prepare_run;
    G_OBJECT_CLASS (klass)->finalize = tf_pango_renderer_finalize;
}


static void tf_pango_renderer_instance_init(tfPangoRenderer *self) {
    self->priv = TF_PANGO_RENDERER_GET_PRIVATE (self);
}

static void tf_pango_renderer_finalize(GObject *obj) {
    tfPangoRenderer *self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, TF_TYPE_PANGO_RENDERER, tfPangoRenderer);
    // don't have ownership of CustomPangoRenderer, so leave it alone
    G_OBJECT_CLASS (tf_pango_renderer_parent_class)->finalize(obj);
}

GType tf_pango_renderer_get_type(void) {
    static volatile gsize tf_pango_renderer_type_id__volatile = 0;
    if (g_once_init_enter (&tf_pango_renderer_type_id__volatile)) {
        static const GTypeInfo g_define_type_info = {sizeof(tfPangoRendererClass), (GBaseInitFunc) NULL,
                                                     (GBaseFinalizeFunc) NULL,
                                                     (GClassInitFunc) tf_pango_renderer_class_init,
                                                     (GClassFinalizeFunc) NULL, NULL, sizeof(tfPangoRenderer), 0,
                                                     (GInstanceInitFunc) tf_pango_renderer_instance_init, NULL};
        GType tf_pango_renderer_type_id;
        tf_pango_renderer_type_id = g_type_register_static(pango_renderer_get_type(), "tfPangoRenderer",
                                                           &g_define_type_info, (GTypeFlags) 0);
        g_once_init_leave (&tf_pango_renderer_type_id__volatile, tf_pango_renderer_type_id);
    }
    return tf_pango_renderer_type_id__volatile;
}

