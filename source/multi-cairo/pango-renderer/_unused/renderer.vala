using Cairo;
using Pango;

namespace tf {
    public class PangoRenderer : Pango.Renderer {
        private Cairo.Context cr;
        public PangoRenderer(Cairo.Context cr) {
            this.cr = cr;
        }
        ~PangoRenderer() {}
        public override void draw_glyphs(Pango.Font font, Pango.GlyphString glyphs, int x, int y) {
        }
        public override void draw_glyph_item(string? text, Pango.GlyphItem glyph_item, int x, int y) {
        }
        public override void draw_rectangle(Pango.RenderPart part, int x, int y, int width, int height) {
        }
        public override void draw_error_underline(int x, int y, int width, int height) {
        }
        public override void prepare_run(Pango.LayoutRun run) {
        }
    }
}
