//
// Created by dang on 8/10/18.
//

#ifndef APIDEMO_CPPRENDERER_H
#define APIDEMO_CPPRENDERER_H

#include <glib.h>
#include <glib-object.h>
#include <pango/pango.h>
#include <cairo.h>
#include <stdlib.h>
#include <string.h>

#include "CustomPangoRenderer.h"

G_BEGIN_DECLS

#define TF_TYPE_PANGO_RENDERER (tf_pango_renderer_get_type ())
#define TF_PANGO_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TF_TYPE_PANGO_RENDERER, tfPangoRenderer))
#define TF_PANGO_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TF_TYPE_PANGO_RENDERER, tfPangoRendererClass))
#define TF_IS_PANGO_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TF_TYPE_PANGO_RENDERER))
#define TF_IS_PANGO_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TF_TYPE_PANGO_RENDERER))
#define TF_PANGO_RENDERER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TF_TYPE_PANGO_RENDERER, tfPangoRendererClass))

typedef struct _tfPangoRenderer tfPangoRenderer;
typedef struct _tfPangoRendererClass tfPangoRendererClass;
typedef struct _tfPangoRendererPrivate tfPangoRendererPrivate;

struct _tfPangoRenderer {
    PangoRenderer parent_instance;
    tfPangoRendererPrivate * priv;
};

struct _tfPangoRendererClass {
    PangoRendererClass parent_class;
};

GType tf_pango_renderer_get_type (void) G_GNUC_CONST;
tfPangoRenderer* tf_pango_renderer_new (CustomPangoRenderer *cpr);
tfPangoRenderer* tf_pango_renderer_construct (GType object_type, CustomPangoRenderer *cpr);

G_END_DECLS

#endif //APIDEMO_CPPRENDERER_H
