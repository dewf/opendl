//
// Created by dang on 9/24/18.
//

#ifndef __CG_CONTEXT_H__
#define __CG_CONTEXT_H__

#include <cstring>
#include <cmath>
#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

#include "CGColor.h"
#include "CGImage.h"
#include "CGPath.h"

#define MAX_DASH_LENGTHS 32

#include "../../util.h"
#include "../../../common/geometry.h"

#include "../../pango-renderer/MyPangoLayout.h" // for run struct

// State:
// ===================================
// CTM (current transformation matrix)
// clip region
// image interpolation quality
// line width
// line join
// miter limit
// line cap
// line dash
// flatness
// should anti-alias
// rendering intent
// fill color space
// stroke color space
// fill color
// stroke color
// alpha value
// font
// font size
// character spacing
// text drawing mode
// shadow parameters
// the pattern phase
// the font smoothing parameter
// blend mode

struct MaskBufferItem {
    int width = -1; // dimensions for both the mask and buffer surfaces
    int height = -1;
    cairo_surface_t *maskSurface = nullptr;
    cairo_surface_t *bufferSurface = nullptr; // temporary drawing surface (instead of root cairo_t* of CGContext), will be applied w/ maskSurface alpha when popped
    cairo_t *crBuffer = nullptr; // buffer context
    dl_CGRect rect; // apply to this area of root
    //
    cairo_t *crLast; // apply buffer through mask to this cr (root generally, but perhaps previous masked layers/buffers)

    void apply() {
        cairo_set_source_surface(crLast, bufferSurface, rect.origin.x, rect.origin.y);
        cairo_mask_surface(crLast, maskSurface, rect.origin.x, rect.origin.y);

        // once I understand move constructors and that sort of thing I can do this right, in the destructor
        cairo_surface_destroy(maskSurface);
        cairo_destroy(crBuffer);
        cairo_surface_destroy(bufferSurface);
        // do not touch crLast! not owned by us
    }
};


struct GraphicState {
    CGColorRef fillColor = CGColor::White; // no need to copy/retain, since eternal
    CGColorRef strokeColor = CGColor::Black; // same
    dl_CGFloat lineWidth = 1.0;
    struct {
        dl_CGFloat phase;
        dl_CGFloat lengths[MAX_DASH_LENGTHS];
        size_t count;
    } dash = {};
    dl_CGTextDrawingMode textDrawingMode = dl_kCGTextFill;
    std::vector<MaskBufferItem> maskStack; // for every call to clipToMask()

    GraphicState() = default;

    GraphicState(const GraphicState& other) {
        // copy state from previous level
        if (other.fillColor) {
            fillColor = other.fillColor->copy();
        }
        if (other.strokeColor) {
            strokeColor = other.strokeColor->copy();
        }
        dash = other.dash;
        lineWidth = other.lineWidth;
        textDrawingMode = other.textDrawingMode;
    }

    ~GraphicState() {
        fillColor->release();
        strokeColor->release();

        while (!maskStack.empty()) {
            maskStack.back().apply();
            maskStack.pop_back();
        }
    }

    cairo_t *getTopBufferContext(cairo_t *fallback) {
        // return the top-most buffer cr for drawing, else none if there are no maskbuffers
        if (!maskStack.empty()) {
            return maskStack.back().crBuffer;
        } else {
            return fallback;
        }
    }

    void restore(cairo_t *cr) {
        // whatever we need to take out of our state and re-apply to the context
        cairo_set_line_width(cr, lineWidth);
        cairo_set_dash(cr, dash.lengths, (int) dash.count, dash.phase);
    }

    void setFillColor(CGColorRef color) {
        auto old = fillColor;
        fillColor = color->copy();
        old->release(); // release after copy in case they're the same object
    }
    void setStrokeColor(CGColorRef color) {
        auto old = strokeColor;
        strokeColor = color->copy();
        old->release(); // release after copy in case they're the same object
    }
};

class CGContext; typedef CGContext* CGContextRef;
class CGContext : public cf::Object {
    cairo_t *rootContext = nullptr; // the one passed in to constructor
    cairo_t *cr = nullptr; // current context, can point to root context or possibly a mask buffer on the mask stack
    std::vector<GraphicState> stateStack;

    // width and height needed for linear gradient clipping
    int width = -1;
    int height = -1;

    // stuff that isn't part of graphics state:
    cairo_matrix_t textMatrix = {};
    dl_CGPoint textPosition = dl_CGPointZero;

    // misc private utils
    void clipTargetByHalfPlane(dl_CGPoint hp_point, dl_CGPoint hp_vec);

    GraphicState *currentState() {
        return &stateStack.back();
    }

    void pushState() {
        stateStack.emplace_back(stateStack.back()); // copy of previous
    }

    void popState() {
        stateStack.pop_back();
        // get active cairo context, which might actually be a mask buffer (ie, buffer where all drawing occurs before mask gets popped / applied)
        cr = stateStack.back().getTopBufferContext(rootContext);
        stateStack.back().restore(cr);
    }

    void fillStrokeClipRun(dl_CGFloat x, dl_CGFloat y, const PangoRunStruct &run, CGColorRef fillColor, CGColorRef strokeColor, dl_CGFloat strokeWidth, bool doClip);
    void commonTextDraw(dl_CGFloat x, dl_CGFloat y, const PangoRunStruct &run, CGColorRef fillColor, CGColorRef strokeColor, dl_CGFloat strokeWidth);
public:
    CGContext(cairo_t *cr, int width, int height)
            : rootContext(cr)
    {
        cairo_matrix_init_identity(&textMatrix);
        stateStack.emplace_back(GraphicState()); // make sure there's 1 on the stack at first
        this->width = width;
        this->height = height;

        currentState()->lineWidth = 1.0;
        currentState()->setFillColor(CGColor::White);
        currentState()->setStrokeColor(CGColor::Black);

        this->cr = rootContext; // initially points to the root, but can refer to other things (mask buffers) as needed
    }

    ~CGContext() override {}

    const char *getTypeName() const override {
        return "CGContext";
    }

    CGContextRef copy() override {
        throw cf::Exception("CGContexts cannot be copied");
    }

    cairo_t *getCairoContext() {
        return cr;
    }

    void translateToTextPosition() {
        cairo_translate(cr, textPosition.x, textPosition.y);
    }

    CGColorRef getFillColor() {
        return currentState()->fillColor;
    }

    // public API methods =============================
    void fillRect(dl_CGRect rect) {
        currentState()->fillColor->apply(cr);
        cairo_rectangle(cr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        cairo_fill(cr);
    }

    void strokeRect(dl_CGRect rect) {
        currentState()->strokeColor->apply(cr);
        cairo_rectangle(cr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        cairo_stroke(cr);
    }

    void strokeRectWithWidth(dl_CGRect rect, dl_CGFloat width) {
        cairo_set_line_width(cr, width);
        strokeRect(rect);
        // restore previous line width
        cairo_set_line_width(cr, currentState()->lineWidth);
    }

    void beginPath() {
        cairo_new_path(cr);
    }

    void closePath() {
        cairo_close_path(cr);
    }

    void addPath(CGPathRef path) {
        path->apply(cr);
    }

    void moveToPoint(dl_CGFloat x, dl_CGFloat y) {
        cairo_move_to(cr, x, y);
    }

    void addLineToPoint(dl_CGFloat x, dl_CGFloat y) {
        cairo_line_to(cr, x, y);
    }

    void addRect(dl_CGRect rect) {
        cairo_rectangle(cr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
    }

    void addArc(dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, int clockwise);

    void addArcToPoint(dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius);

    void drawPath(dl_CGPathDrawingMode mode) {
        if (mode == dl_kCGPathFill || mode == dl_kCGPathFillStroke) {
            currentState()->fillColor->apply(cr);
            cairo_fill_preserve(cr);
            // note path still exists because of _preserve
        }
        if (mode == dl_kCGPathStroke || mode == dl_kCGPathFillStroke) {
            currentState()->strokeColor->apply(cr);
            cairo_stroke(cr);
        } else {
            // clear path because this path didn't clear it
            cairo_new_path(cr);
        }
    }

    void strokePath()
    {
        currentState()->strokeColor->apply(cr);
        cairo_stroke(cr);
    }

    void fillPath() {
        currentState()->fillColor->apply(cr);
        cairo_fill(cr);
    }

    void setLineWidth(dl_CGFloat width) {
        currentState()->lineWidth = width;
        cairo_set_line_width(cr, width);
    }

    void setLineDash(dl_CGFloat phase, const dl_CGFloat *lengths, size_t count) {
        assert(count < MAX_DASH_LENGTHS);
        currentState()->dash.phase = phase;
        memcpy(&currentState()->dash.lengths, lengths, sizeof(dl_CGFloat) * count);
        currentState()->dash.count = count;

        cairo_set_dash(cr, lengths, (int) count, phase);
    }

    dl_CGAffineTransform getMatrix() {
        cairo_matrix_t crMatrix;
        cairo_get_matrix(cr, &crMatrix);
        return cairo_to_dl_matrix(crMatrix);
    }

    void setMatrix(dl_CGAffineTransform t) {
        auto crMatrix = dl_to_cairo_matrix(t);
        cairo_set_matrix(cr, &crMatrix);
    }

    void translateCTM(dl_CGFloat tx, dl_CGFloat ty) {
        cairo_translate(cr, tx, ty);
    }

    void rotateCTM(dl_CGFloat angle) {
        cairo_rotate(cr, angle);
    }

    void scaleCTM(dl_CGFloat sx, dl_CGFloat sy) {
        cairo_scale(cr, sx, sy);
    }

    void concatCTM(dl_CGAffineTransform transform) {
        cairo_matrix_t m = dl_to_cairo_matrix(transform);
        cairo_transform(cr, &m);
    }

    void clip() {
        cairo_clip(cr);
    }

    void clipToRect(dl_CGRect rect) {
        cairo_rectangle(cr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        cairo_clip(cr);
    }

    void saveGState() {
        cairo_save(cr);
        pushState();
    }

    void restoreGState() {
        popState();
        cairo_restore(cr);
    }

    // colors
    void setRGBFillColor(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha) {
        auto c = new CGColor(red, green, blue, alpha);
        currentState()->setFillColor(c);
        c->release();
    }
    void setRGBStrokeColor(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha) {
        auto c = new CGColor(red, green, blue, alpha);
        currentState()->setStrokeColor(c);
        c->release();
    }
    void setFillColorWithColor(dl_CGColorRef color)
    {
        currentState()->setFillColor((CGColorRef)color);
    }
    void setStrokeColorWithColor(dl_CGColorRef color)
    {
        currentState()->setStrokeColor((CGColorRef)color);
    }

    // gradient
    void drawLinearGradient(dl_CGGradientRef gradient, dl_CGPoint startPoint, dl_CGPoint endPoint,
                                dl_CGGradientDrawingOptions options);

    // text
    void setTextMatrix(dl_CGAffineTransform t)
    {
        textMatrix = dl_to_cairo_matrix(t);
    }
    void setTextPosition(dl_CGFloat x, dl_CGFloat y)
    {
        textPosition = dl_CGPointMake(x, y);
    }
    void setTextDrawingMode(dl_CGTextDrawingMode mode)
    {
        currentState()->textDrawingMode = mode;
    }

    void drawGlyphRun(const PangoRunStruct &run, dl_CGFloat x, dl_CGFloat y);

    dl_CGFloat getTextScaleRatio() {
        cairo_matrix_t ctm;
        cairo_get_matrix(cr, &ctm);
        auto globalMean = sqrt(fabs(ctm.xx * ctm.yy));
        auto textMean = sqrt(fabs(textMatrix.xx * textMatrix.yy));
        return globalMean / textMean;
    }

    // image
    void clipToMask(dl_CGRect rect, CGImageRef mask);
    void drawImage(dl_CGRect rect, CGImageRef image);

};

#endif //__CG_CONTEXT_H__
