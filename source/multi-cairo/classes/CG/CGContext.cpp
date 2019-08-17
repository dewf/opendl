//
// Created by dang on 9/24/18.
//

#include "CGContext.h"

#include "../../../common/geometry.h"

#include "CGGradient.h"

#include <algorithm>
#include <math.h>

void CGContext::clipTargetByHalfPlane(dl_CGPoint hp_point, dl_CGPoint hp_vec) {
    dl_CGRect rect = {{0,               0},
                   {(dl_CGFloat) width, (dl_CGFloat) height}};

    dl_CGPoint clipPoints[10]; // pretty sure the mathematical max is 5, but just in case ...
    int clipPointCount = 0;
    clipRectByHalfPlane(rect, hp_point, hp_vec, clipPoints, &clipPointCount);

    beginPath();
    moveToPoint(clipPoints[0].x, clipPoints[0].y);
    for (int i = 1; i < clipPointCount; i++) {
        addLineToPoint(clipPoints[i].x, clipPoints[i].y);
    }

    closePath();
    clip();
}

void CGContext::drawLinearGradient(dl_CGGradientRef gradient, dl_CGPoint startPoint, dl_CGPoint endPoint, dl_CGGradientDrawingOptions options)
{
    auto pattern = cairo_pattern_create_linear(startPoint.x, startPoint.y, endPoint.x, endPoint.y);

    ((CGGradientRef) gradient)->writeToPattern(pattern);
//    for (size_t i = 0; i < gradient->count; i++) {
//        _gradComponent &gc = gradient->components[i];
//        cairo_pattern_add_color_stop_rgba(pattern, gc.loc, gc.r, gc.g, gc.b, gc.a);
//    }

    // save because we're about to create a clip mask
    saveGState();

    dl_CGPoint vec = {endPoint.x - startPoint.x, endPoint.y - startPoint.y};
    if (!(options & dl_kCGGradientDrawsBeforeStartLocation)) {
        clipTargetByHalfPlane({startPoint.x, startPoint.y}, vec);
    }
    if (!(options & dl_kCGGradientDrawsAfterEndLocation)) {
        dl_CGPoint vec2 = {-vec.x, -vec.y};
        clipTargetByHalfPlane({endPoint.x, endPoint.y}, vec2);
    }
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_set_source(cr, pattern);
    cairo_fill(cr);

    restoreGState();
    // end clip mask

    // free
    cairo_pattern_destroy(pattern);
}

static void luminance_to_alpha(cairo_surface_t *surface, int width, int height) {
    //16843009 (UINT_MAX / 255)
    // * 0.2126 =  3580823.7134 (r coeff)
    // * 0.7152 = 12046120.0368 (g coeff)
    // * 0.0722 =  1216065.2498 (b coeff)
    auto const r_coeff = (uint32_t) 3580823 + 1; // the +1 is to account for the roundoff error (lost fractional parts)
    auto const g_coeff = (uint32_t) 12046120;
    auto const b_coeff = (uint32_t) 1216065;

    auto data = (uint32_t *) cairo_image_surface_get_data(surface);
    for (int i = 0; i < width * height; i++) {
        auto pixel = *data;
        auto r = (uint8_t)(pixel & 0xFF);
        auto g = (uint8_t)((pixel & 0xFF00) >> 8);
        auto b = (uint8_t)((pixel & 0xFF0000) >> 16);
        uint32_t alpha = (r * r_coeff) + (g * g_coeff) + (b * b_coeff); // essentially a fixed point multiplication where we only care about the top 8 bits of the result
        pixel &= 0x00FFFFFF; // clear any existing alpha on the target pixel
        pixel |= alpha & 0xFF000000; // set pixel alpha to top 8 bits of calculated luminance
        (*data++) = pixel;
    }
}

void CGContext::clipToMask(dl_CGRect rect, CGImageRef mask)
{
    int surfaceWidth, surfaceHeight;
    auto surface = mask->getSurface(&surfaceWidth, &surfaceHeight);
    luminance_to_alpha(surface, surfaceWidth, surfaceHeight);

    // now create a temporary buffer surface which will receive all subsequent drawing,
    //   until mask is popped, at which point the mask will be used to combine the buffer with our existing surface
    // buffer is then destroyed
    MaskBufferItem mbi;
    mbi.width = surfaceWidth;
    mbi.height = surfaceHeight;
    mbi.maskSurface = cairo_surface_reference(surface);
    mbi.bufferSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, surfaceWidth, surfaceHeight);
    mbi.crBuffer = cairo_create(mbi.bufferSurface);
    mbi.rect = rect;
    mbi.crLast = cr; // whatever the current one is

    cr = mbi.crBuffer; // redirect all drawing to that buffer surface (which will be applied to crLast upon state pop)

    currentState()->maskStack.emplace_back(mbi);
}

template <typename T>
inline T ELVIS(T x, T y) {
    if (x) {
        return x;
    } else {
        return y;
    }
}

void CGContext::drawGlyphRun(const PangoRunStruct &run, dl_CGFloat x, dl_CGFloat y) {
    if (run.attrs) {
        // foreground fallback logic:
        // use specific foreground brush color if specified,
        // otherwise use the context foreground if it's specifically asked for,
        // otherwise use the default brush (probably black)
        CGColorRef fgColor = ELVIS(run.attrs->getForegroundColor(), ELVIS(run.attrs->getForegroundColorFromContext(this), CGColor::Black));

        float strokeWidth;
        if (run.attrs->getStrokeWidth(&strokeWidth)) {
            // stroke attribute present, override textDrawingMode (and choose to fill or not based on stroke width sign)
            auto strokeColor = ELVIS(run.attrs->getStrokeColor(), CGColor::Black); // ? should there be another fallback here? foreground color etc?
            strokeWidth /= (float)getTextScaleRatio();

            if (strokeWidth < 0) {
                // if negative stroke width, fill the text with the foreground (or default) color and stroke with the stroke color (if any)
                fillStrokeClipRun(x, y, run, fgColor, strokeColor,
                        -strokeWidth, // convert from negative
                        false // clipping always off when stroke specified
                        );
            } else {
                // if positive stroke width, stroke the text with foreground (or default) color
                fillStrokeClipRun(x, y, run,
                        nullptr, // stroke only
                        strokeColor,
                        strokeWidth,
                        false // clipping always off when stroke specified
                        );
            }
        } else {
            // strokewidth wasn't provided, use textDrawingMode (and context line width)
            auto strokeColor = ELVIS(run.attrs->getStrokeColor(), ELVIS(currentState()->strokeColor, CGColor::Black));
            strokeWidth = (float)(currentState()->lineWidth / getTextScaleRatio());

            commonTextDraw(x, y, run, fgColor, strokeColor, strokeWidth);
        }
    } else {
        // vanilla rendering based on text mode (glyph range had no dict/attributes)
        // but then how would font/size/etc be defined?
        auto strokeWidth = (float)(currentState()->lineWidth / getTextScaleRatio());
        commonTextDraw(x, y, run, currentState()->fillColor, currentState()->strokeColor, strokeWidth);
    }
}

void CGContext::fillStrokeClipRun(dl_CGFloat x, dl_CGFloat y, const PangoRunStruct &run, CGColorRef fillColor, CGColorRef strokeColor,
                                  dl_CGFloat strokeWidth, bool doClip)
{
    if (fillColor) {
        cairo_move_to(cr, x, y);
        fillColor->apply(cr);
        pango_cairo_glyph_string_path(cr, run.run->item->analysis.font, run.run->glyphs);
        cairo_fill(cr);
    }

    if (strokeColor) {
        cairo_move_to(cr, x, y);
        strokeColor->apply(cr);
        pango_cairo_glyph_string_path(cr, run.run->item->analysis.font, run.run->glyphs);
        cairo_set_line_width(cr, strokeWidth);
        cairo_stroke(cr);
    }

    if (doClip) {
        // TODO: this needs to be accumulated over multiple runs, possibly multiple lines, controlled by the frame / line stuff
        cairo_move_to(cr, x, y);
        pango_cairo_glyph_string_path(cr, run.run->item->analysis.font, run.run->glyphs);
        cairo_clip(cr);
    }
}

void CGContext::commonTextDraw(dl_CGFloat x, dl_CGFloat y, const PangoRunStruct &run, CGColorRef fillColor, CGColorRef strokeColor,
                               dl_CGFloat strokeWidth)
{
    auto mode = currentState()->textDrawingMode;
    switch (mode) {
        case dl_kCGTextFill:
        case dl_kCGTextFillClip:
            cairo_move_to(cr, x, y);
            fillColor->apply(cr);
            pango_cairo_glyph_string_path(cr, run.run->item->analysis.font, run.run->glyphs);
            cairo_fill(cr);
            break;
        case dl_kCGTextStroke:
        case dl_kCGTextStrokeClip:
            fillStrokeClipRun(x, y, run, nullptr, strokeColor, strokeWidth, (mode == dl_kCGTextStrokeClip));
            break;
        case dl_kCGTextFillStroke:
        case dl_kCGTextFillStrokeClip:
            fillStrokeClipRun(x, y, run, fillColor, strokeColor, strokeWidth, (mode == dl_kCGTextFillStrokeClip));
            break;
        case dl_kCGTextClip:
            fillStrokeClipRun(x, y, run, nullptr, nullptr, 0, true);
            break;
        case dl_kCGTextInvisible:
            // just advance the text drawing point
            break;
        default:
            printf("unknown text drawing mode %d\n", mode);
    }
}

void CGContext::addArc(dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, int clockwise)
{
    if (clockwise) {
        // end must be greater than start, otherwise cairo alters the end (by repeatedly adding 2PI)
        if (startAngle > endAngle) {
            std::swap(startAngle, endAngle);
        }
        cairo_arc(cr, x, y, radius, startAngle, endAngle);
    } else {
        // start must be greater than end, otherwise cairo alters the end (by repeatedly subtracting 2PI)
        if (startAngle < endAngle) {
            std::swap(startAngle, endAngle);
        }
        cairo_arc_negative(cr, x, y, radius, startAngle, endAngle);
    }
}

void CGContext::addArcToPoint(dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius)
{
    // draw from current point to (almost) x1,y1 - create arc of specified radius there, tangent to line from x1,y1 to x2,y2
    // find angle between two lines:
    double x0, y0; // current point
    cairo_get_current_point(cr, &x0, &y0);

    double centerX, centerY, startX, startY, startAngle, endAngle;
    calcArcToPoint(x0, y0, x1, y1, x2, y2, radius, &centerX, &centerY, &startX, &startY, &startAngle, &endAngle);

    cairo_line_to(cr, startX, startY);
    cairo_arc(cr, centerX, centerY, radius, startAngle, endAngle);
    // we end at the end of that arc, resume drawing from there
}

void CGContext::drawImage(dl_CGRect rect, CGImageRef image)
{
    int surfaceWidth, surfaceHeight;
    auto surface = image->getSurface(&surfaceWidth, &surfaceHeight);

    cairo_set_source_surface(cr, surface, rect.origin.x, rect.origin.y);
    // don't we need to scale, to fit full content of surfaceWidth/Height in rect.size ?
    cairo_rectangle(cr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
    cairo_fill(cr);
}


