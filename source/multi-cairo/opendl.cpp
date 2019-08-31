#include "../opendl.h"
// opendl already includes cairo

#include <vector>
#include <string>
#include <glib.h>
#include <cstring>

#include <cassert>
#include <cairo.h>
#include <pango/pangocairo.h>
#include <cmath>

#include "pango-renderer/StringAttrManager.h"
#include "pango-renderer/CoreTextAttrManager.h"

#include <map>

#include "../common/geometry.h"

#include "../common/classes/CF/CFTypes.h"

#include "classes/CG/CGContext.h"
#include "classes/CG/CGBitmapContext.h"
#include "classes/CG/CGColor.h"
#include "classes/CG/CGColorSpace.h"
#include "classes/CG/CGGradient.h"
#include "classes/CG/CGPath.h"

#include "classes/CT/CTFont.h"
#include "classes/CT/CTLine.h"
#include "classes/CT/CTFrameSetter.h"

#include "classes/CT/CTParagraphStyle.h"

#include "util.h"

const dl_CFIndex dl_kCFNotFound = -1;
const dl_CGPoint dl_CGPointZero = dl_CGPointMake(0, 0);
const dl_CGRect dl_CGRectZero = dl_CGRectMake(0, 0, 0, 0);
const dl_CGAffineTransform dl_CGAffineTransformIdentity = { 1, 0, 0, 1, 0, 0 }; // a = 1, b = 0, c = 0, d = 1, tx = 0, ty = 0

OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformInvert(dl_CGAffineTransform t)
{
    auto mat = dl_to_cairo_matrix(t);
    cairo_matrix_invert(&mat);
    return cairo_to_dl_matrix(mat);
}

OPENDL_API int CDECL dl_Init(dl_PlatformOptions *options)
{
    // nothing yet
}

OPENDL_API void CDECL dl_Shutdown()
{
    // nothing yet
}

OPENDL_API dl_CGContextRef CDECL dl_CGContextCreateCairo(cairo_t *cr, int width, int height) {
    return (dl_CGContextRef) new CGContext(cr, width, height);
}

OPENDL_API void dl_CGContextRelease(dl_CGContextRef c)
{
    ((CGContextRef)c)->release();
}

OPENDL_API void CDECL
dl_CGContextSetRGBFillColor(dl_CGContextRef c, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha) {
    ((CGContextRef)c)->setRGBFillColor(red, green, blue, alpha);
}

OPENDL_API void CDECL dl_CGContextFillRect(dl_CGContextRef c, dl_CGRect rect) {
    ((CGContextRef)c)->fillRect(rect);
}

OPENDL_API void CDECL
dl_CGContextSetRGBStrokeColor(dl_CGContextRef c, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha) {
    ((CGContextRef)c)->setRGBStrokeColor(red, green, blue, alpha);
}

OPENDL_API void CDECL dl_CGContextStrokeRect(dl_CGContextRef c, dl_CGRect rect) {
    ((CGContextRef)c)->strokeRect(rect);
}

OPENDL_API void CDECL dl_CGContextStrokeRectWithWidth(dl_CGContextRef c, dl_CGRect rect, dl_CGFloat width) {
    ((CGContextRef)c)->strokeRectWithWidth(rect, width);
}

OPENDL_API void CDECL dl_CGContextBeginPath(dl_CGContextRef c) {
    ((CGContextRef)c)->beginPath();
}

OPENDL_API void CDECL dl_CGContextClosePath(dl_CGContextRef c) {
    ((CGContextRef)c)->closePath();
}

OPENDL_API void CDECL dl_CGContextMoveToPoint(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y) {
    ((CGContextRef)c)->moveToPoint(x, y);
}

OPENDL_API void CDECL dl_CGContextAddLineToPoint(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y) {
    ((CGContextRef)c)->addLineToPoint(x, y);
}

OPENDL_API void CDECL dl_CGContextAddRect(dl_CGContextRef c, dl_CGRect rect) {
    ((CGContextRef)c)->addRect(rect);
}

OPENDL_API void CDECL
dl_CGContextAddArc(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle,
                int clockwise) {
    ((CGContextRef)c)->addArc(x, y, radius, startAngle, endAngle, clockwise);
}

OPENDL_API void CDECL dl_CGContextAddArcToPoint(dl_CGContextRef c, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius)
{
    ((CGContextRef)c)->addArcToPoint(x1, y1, x2, y2, radius);
}

OPENDL_API void CDECL dl_CGContextDrawPath(dl_CGContextRef c, dl_CGPathDrawingMode mode) {
    ((CGContextRef)c)->drawPath(mode);
}

OPENDL_API void CDECL dl_CGContextStrokePath(dl_CGContextRef c) {
    ((CGContextRef)c)->strokePath();
}

OPENDL_API void CDECL dl_CGContextFillPath(dl_CGContextRef c) {
    ((CGContextRef)c)->fillPath();
}

OPENDL_API void CDECL dl_CGContextSetLineWidth(dl_CGContextRef c, dl_CGFloat width) {
    ((CGContextRef)c)->setLineWidth(width);
}

OPENDL_API void CDECL dl_CGContextSetLineDash(dl_CGContextRef c, dl_CGFloat phase, const dl_CGFloat *lengths, size_t count) {
    ((CGContextRef)c)->setLineDash(phase, lengths, count);
}

OPENDL_API void CDECL dl_CGContextTranslateCTM(dl_CGContextRef c, dl_CGFloat tx, dl_CGFloat ty) {
    ((CGContextRef)c)->translateCTM(tx, ty);
}

OPENDL_API void CDECL dl_CGContextRotateCTM(dl_CGContextRef c, dl_CGFloat angle) {
    ((CGContextRef)c)->rotateCTM(angle);
}

OPENDL_API void CDECL dl_CGContextScaleCTM(dl_CGContextRef c, dl_CGFloat sx, dl_CGFloat sy) {
    ((CGContextRef)c)->scaleCTM(sx, sy);
}

OPENDL_API void CDECL dl_CGContextConcatCTM(dl_CGContextRef c, dl_CGAffineTransform transform) {
    ((CGContextRef)c)->concatCTM(transform);
}

OPENDL_API void CDECL dl_CGContextClip(dl_CGContextRef c) {
    ((CGContextRef)c)->clip();
}

OPENDL_API void CDECL dl_CGContextClipToRect(dl_CGContextRef c, dl_CGRect rect) {
    ((CGContextRef)c)->clipToRect(rect);
}

OPENDL_API void dl_CGContextSaveGState(dl_CGContextRef c) {
    ((CGContextRef)c)->saveGState();
}

OPENDL_API void dl_CGContextRestoreGState(dl_CGContextRef c) {
    ((CGContextRef)c)->restoreGState();
}

// path stuff ===========================================
OPENDL_API dl_CGMutablePathRef CDECL dl_CGPathCreateMutable(void)
{
    return (dl_CGMutablePathRef)CGMutablePath::create();
}

OPENDL_API void CDECL dl_CGPathAddRect(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGRect rect)
{
    ((CGMutablePathRef)path)->addRect(m, rect);
}

OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform)
{
    return (dl_CGPathRef)CGPath::createWithRect(rect, transform);
}

OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform)
{
    return (dl_CGPathRef)CGPath::createWithEllipseInRect(rect, transform);
}

OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateWithRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform)
{
    return (dl_CGPathRef)CGPath::createWithRoundedRect(rect, cornerWidth, cornerHeight, transform);
}

OPENDL_API void CDECL dl_CGPathRelease(dl_CGPathRef path)
{
    ((CGPathRef)path)->release();
}

OPENDL_API void CDECL dl_CGContextAddPath(dl_CGContextRef context, dl_CGPathRef path)
{
    ((CGContextRef)context)->addPath((CGPathRef)path);
}

//gchar *path_to_uri(gchar *path) {
//    gchar *absolute;
//    if (g_path_is_absolute(path)) {
//        absolute = g_strdup(path);
//    } else {
//        gchar *dir = g_get_current_dir();
//        absolute = g_build_filename(dir, path, (gchar *) 0);
//        free(dir);
//    }
//    GError *error = nullptr;
//    auto uri = g_filename_to_uri(absolute, nullptr, &error);
//    free(absolute);
//    if (!uri) {
//        printf("uri creation failure: %s\n", error->message);
//        return nullptr;
//    }
//    return uri;
//}

/* colorspace and gradient stuff */

const dl_CFStringRef dl_kCGColorSpaceGenericGray = dl_CFSTR("dl_kCGColorSpaceGenericGray");
const dl_CFStringRef dl_kCGColorSpaceGenericRGB = dl_CFSTR("dl_kCGColorSpaceGenericRGB");
const dl_CFStringRef dl_kCGColorSpaceGenericCMYK = dl_CFSTR("dl_kCGColorSpaceGenericCMYK");
const dl_CFStringRef dl_kCGColorSpaceGenericRGBLinear = dl_CFSTR("dl_kCGColorSpaceGenericRGBLinear");
const dl_CFStringRef dl_kCGColorSpaceAdobeRGB1998 = dl_CFSTR("dl_kCGColorSpaceAdobeRGB1998");
const dl_CFStringRef dl_kCGColorSpaceSRGB = dl_CFSTR("dl_kCGColorSpaceSRGB");
const dl_CFStringRef dl_kCGColorSpaceGenericGrayGamma2_2 = dl_CFSTR("dl_kCGColorSpaceGenericGrayGamma2_2");

OPENDL_API dl_CGColorSpaceRef CDECL dl_CGColorSpaceCreateWithName(dl_CFStringRef name) {
    return (dl_CGColorSpaceRef) new CGColorSpace((cf::StringRef)name);
}

OPENDL_API dl_CGColorSpaceRef CDECL dl_CGColorSpaceCreateDeviceGray(void)
{
    return (dl_CGColorSpaceRef) new CGColorSpace(CGColorSpace::DeviceSpace_Gray);
}

OPENDL_API void CDECL dl_CGColorSpaceRelease(dl_CGColorSpaceRef colorSpace)
{
    ((CGColorSpaceRef)colorSpace)->release();
}

// color stuff ==============

OPENDL_API dl_CGColorRef CDECL dl_CGColorCreateGenericRGB(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha)
{
    return (dl_CGColorRef) new CGColor(red, green, blue, alpha);
}

OPENDL_API void CDECL dl_CGColorRelease(dl_CGColorRef color)
{
    ((CGColorRef)color)->release();
}

OPENDL_API void CDECL dl_CGContextSetFillColorWithColor(dl_CGContextRef c, dl_CGColorRef color)
{
    ((CGContextRef)c)->setFillColorWithColor(color);
}

OPENDL_API void CDECL dl_CGContextSetStrokeColorWithColor(dl_CGContextRef c, dl_CGColorRef color)
{
    ((CGContextRef)c)->setStrokeColorWithColor(color);
}

const dl_CFStringRef dl_kCGColorWhite = dl_CFSTR("dl_kCGColorWhite");
const dl_CFStringRef dl_kCGColorBlack = dl_CFSTR("dl_kCGColorBlack");
const dl_CFStringRef dl_kCGColorClear = dl_CFSTR("dl_kCGColorClear");
OPENDL_API dl_CGColorRef CDECL dl_CGColorGetConstantColor(dl_CFStringRef colorName)
{
    if (colorName == dl_kCGColorWhite) {
        return (dl_CGColorRef)CGColor::White;
    } else if (colorName == dl_kCGColorBlack) {
        return (dl_CGColorRef)CGColor::Black;
    } else if (colorName == dl_kCGColorClear) {
        return (dl_CGColorRef)CGColor::Clear;
    } else {
        printf("dl_CGColorGetConstantColor asked for unknown color - returning null");
        return nullptr;
    }
}

OPENDL_API size_t CDECL dl_CGColorGetNumberOfComponents(dl_CGColorRef color)
{
    return ((CGColorRef)color)->getNumberOfComponents();
}

OPENDL_API const dl_CGFloat* CDECL dl_CGColorGetComponents(dl_CGColorRef color)
{
    return ((CGColorRef)color)->getComponents();
}

// gradients =======

OPENDL_API dl_CGGradientRef CDECL
dl_CGGradientCreateWithColorComponents(dl_CGColorSpaceRef space, const dl_CGFloat components[], const dl_CGFloat locations[],
                                    size_t count) {
    return (dl_CGGradientRef) CGGradient::createWithColorComponents((CGColorSpaceRef)space, components, locations, count);
}

OPENDL_API void CDECL dl_CGGradientRelease(dl_CGGradientRef gradient) {
    ((CGGradientRef)gradient)->release();
}

OPENDL_API void CDECL
dl_CGContextDrawLinearGradient(dl_CGContextRef c, dl_CGGradientRef gradient, dl_CGPoint startPoint, dl_CGPoint endPoint,
                            dl_CGGradientDrawingOptions options)
{
    ((CGContextRef)c)->drawLinearGradient(gradient, startPoint, endPoint, options);
}

// text =============================
const dl_CFStringRef dl_kCTForegroundColorAttributeName = dl_CFSTR("dl_kCTForegroundColorAttributeName");
const dl_CFStringRef dl_kCTForegroundColorFromContextAttributeName = dl_CFSTR("dl_kCTForegroundColorFromContextAttributeName");
const dl_CFStringRef dl_kCTFontAttributeName = dl_CFSTR("dl_kCTFontAttributeName");
const dl_CFStringRef dl_kCTStrokeWidthAttributeName = dl_CFSTR("dl_kCTStrokeWidthAttributeName");
const dl_CFStringRef dl_kCTStrokeColorAttributeName = dl_CFSTR("dl_kCTStrokeColorAttributeName");

OPENDL_API void dl_CGContextSetTextMatrix(dl_CGContextRef c, dl_CGAffineTransform t)
{
    ((CGContextRef)c)->setTextMatrix(t);
}

OPENDL_API dl_CTFramesetterRef CDECL dl_CTFramesetterCreateWithAttributedString(dl_CFAttributedStringRef attrString)
{
    return (dl_CTFramesetterRef) new CTFrameSetter((cf::AttributedStringRef)attrString);
}

OPENDL_API dl_CTFrameRef CDECL
dl_CTFramesetterCreateFrame(dl_CTFramesetterRef framesetter, dl_CFRange stringRange, dl_CGPathRef path, dl_CFDictionaryRef frameAttributes)
{
    return (dl_CTFrameRef)((CTFrameSetterRef)framesetter)->createFrame(stringRange, (CGPathRef)path, (cf::DictionaryRef)frameAttributes);
}

OPENDL_API dl_CFArrayRef CDECL dl_CTFrameGetLines(dl_CTFrameRef frame)
{
    return (dl_CFArrayRef)((CTFrameRef)frame)->getLines();
}

OPENDL_API void CDECL dl_CTFrameGetLineOrigins(dl_CTFrameRef frame, dl_CFRange range, dl_CGPoint origins[])
{
    ((CTFrameRef)frame)->getLineOrigins(range, origins);
}

OPENDL_API dl_CFDictionaryRef CDECL dl_CTRunGetAttributes(dl_CTRunRef run)
{
    (dl_CFDictionaryRef)((CTRunRef)run)->getAttributes();
}

OPENDL_API double CDECL dl_CTRunGetTypographicBounds(dl_CTRunRef run, dl_CFRange range, dl_CGFloat* ascent, dl_CGFloat* descent, dl_CGFloat* leading)
{
    return ((CTRunRef)run)->getTypographicBounds(range, ascent, descent, leading);
}

OPENDL_API dl_CFRange CDECL dl_CTRunGetStringRange(dl_CTRunRef run)
{
    return ((CTRunRef)run)->getStringRange();
}

OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateWithName(dl_CFStringRef name, dl_CGFloat size, const dl_CGAffineTransform *matrix)
{
    auto nameStr = ((cf::StringRef)name)->getStdString();
    return (dl_CTFontRef) new CTFont(nameStr, size, matrix);
}

OPENDL_API dl_CFArrayRef CDECL dl_CTFontManagerCreateFontDescriptorsFromURL(dl_CFURLRef fileURL)
{
    return (dl_CFArrayRef) CTFontDescriptor::createFontDescriptorsFromURL((cf::URLRef)fileURL);
}

OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateWithFontDescriptor(dl_CTFontDescriptorRef descriptor, dl_CGFloat size, const dl_CGAffineTransform *matrix)
{
    return (dl_CTFontRef) CTFont::createWithFontDescriptor((CTFontDescriptorRef)descriptor, size, matrix);
}

OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateCopyWithSymbolicTraits(dl_CTFontRef font, dl_CGFloat size, const dl_CGAffineTransform *matrix, dl_CTFontSymbolicTraits symTraitValue, dl_CTFontSymbolicTraits symTraitMask)
{
    return (dl_CTFontRef)CTFont::createCopyWithSymbolicTraits((CTFontRef)font, size, matrix, symTraitValue, symTraitMask);
}

OPENDL_API dl_CGFloat CDECL dl_CTFontGetUnderlineThickness(dl_CTFontRef font)
{
    return ((CTFontRef)font)->getUnderlineThickness();
}

OPENDL_API dl_CGFloat CDECL dl_CTFontGetUnderlinePosition(dl_CTFontRef font)
{
    return ((CTFontRef)font)->getUnderlinePosition();
}

OPENDL_API dl_CTRunStatus CDECL dl_CTRunGetStatus(dl_CTRunRef run)
{
    return ((CTRunRef)run)->getRunStatus();
}

OPENDL_API void CDECL dl_CTFrameDraw(dl_CTFrameRef frame, dl_CGContextRef context)
{
    ((CTFrameRef)frame)->draw((CGContextRef)context);
}



OPENDL_API dl_CTParagraphStyleRef CDECL dl_CTParagraphStyleCreate(const dl_CTParagraphStyleSetting * settings, size_t settingCount)
{
    return (dl_CTParagraphStyleRef) CTParagraphStyle::create(settings, settingCount);
}

const dl_CFStringRef dl_kCTParagraphStyleAttributeName = dl_CFSTR("dl_kCTParagraphStyleAttributeName");




OPENDL_API dl_CTLineRef CDECL dl_CTLineCreateWithAttributedString(dl_CFAttributedStringRef string)
{
    return (dl_CTLineRef) CTLine::createWithAttributedString((cf::AttributedStringRef)string);
}

OPENDL_API dl_CGRect CDECL dl_CTLineGetBoundsWithOptions(dl_CTLineRef line, dl_CTLineBoundsOptions options)
{
    return ((CTLineRef)line)->getBoundsWithOptions(options);
}

OPENDL_API double CDECL dl_CTLineGetTypographicBounds(dl_CTLineRef line, dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *leading)
{
    return ((CTLineRef)line)->getTypographicBounds(ascent, descent, leading);
}

OPENDL_API dl_CGFloat CDECL dl_CTLineGetOffsetForStringIndex(dl_CTLineRef line, dl_CFIndex charIndex, dl_CGFloat* secondaryOffset)
{
    return ((CTLineRef)line)->getOffsetForStringIndex(charIndex, secondaryOffset);
}

OPENDL_API dl_CFIndex CDECL dl_CTLineGetStringIndexForPosition(dl_CTLineRef line, dl_CGPoint position)
{
    return ((CTLineRef)line)->getStringIndexForPosition(position);
}

OPENDL_API dl_CFRange CDECL dl_CTLineGetStringRange(dl_CTLineRef line)
{
    return ((CTLineRef)line)->getStringRange();
}

OPENDL_API dl_CFArrayRef CDECL dl_CTLineGetGlyphRuns(dl_CTLineRef line)
{
    return (dl_CFArrayRef)((CTLineRef)line)->getGlyphRuns();
}

OPENDL_API void CDECL dl_CTLineDraw(dl_CTLineRef line, dl_CGContextRef context)
{
    ((CTLineRef)line)->draw((CGContextRef)context);
}

OPENDL_API void CDECL dl_CGContextSetTextPosition(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y)
{
    ((CGContextRef)c)->setTextPosition(x, y);
}

OPENDL_API void CDECL dl_CGContextSetTextDrawingMode(dl_CGContextRef c, dl_CGTextDrawingMode mode)
{
    ((CGContextRef)c)->setTextDrawingMode(mode);
}

OPENDL_API dl_CGContextRef CDECL dl_CGBitmapContextCreate(void *data, size_t width,
                                                    size_t height, size_t bitsPerComponent, size_t bytesPerRow,
                                                    dl_CGColorSpaceRef space, dl_CGBitmapInfo bitmapInfo)
{
    return (dl_CGContextRef) CGBitmapContext::create(data, width, height, bitsPerComponent, bytesPerRow, (CGColorSpaceRef)space, bitmapInfo);
}

OPENDL_API void * CDECL dl_CGBitmapContextGetData(dl_CGContextRef bitmap)
{
    return ((CGBitmapContextRef)bitmap)->getData();
}

OPENDL_API void CDECL dl_CGBitmapContextReleaseData(dl_CGContextRef bitmap)
{
    ((CGBitmapContextRef)bitmap)->releaseData();
}

OPENDL_API dl_CGImageRef CDECL dl_CGBitmapContextCreateImage(dl_CGContextRef context)
{
    // takes a snapshot of a bitmap context, to be used as a mask or what-have-you
    // wrapper around a new surface which can be used as a drawing/mask source
    return (dl_CGImageRef) ((CGBitmapContextRef)context)->createImage();
}

OPENDL_API void dl_CGImageRelease(dl_CGImageRef image)
{
    ((CGImageRef)image)->release();
}

OPENDL_API void CDECL dl_CGContextClipToMask(dl_CGContextRef c, dl_CGRect rect, dl_CGImageRef mask)
{
    ((CGContextRef)c)->clipToMask(rect, (CGImageRef)mask);
}

OPENDL_API void CDECL dl_CGContextDrawImage(dl_CGContextRef c, dl_CGRect rect, dl_CGImageRef image)
{
    ((CGContextRef)c)->drawImage(rect, (CGImageRef)image);
}

// old font page code below ======================
// ===============================================

//dl_CGGradientRef easyGrad(dl_CGFloat fromR, dl_CGFloat fromG, dl_CGFloat fromB, dl_CGFloat fromA,
//                       dl_CGFloat toR, dl_CGFloat toG, dl_CGFloat toB, dl_CGFloat toA) {
//    static auto colorSpace = dl_CGColorSpaceCreateWithName(dl_kCGColorSpaceGenericRGB);
//
//    dl_CGFloat components[] = {
//            fromR, fromG, fromB, fromA,
//            toR, toG, toB, toA
//    };
//    dl_CGFloat locations[] = {
//            0.0, 1.0
//    };
//    return dl_CGGradientCreateWithColorComponents(colorSpace, components, locations, 2);
//}
//
//void gradientRectAround(dl_CGContextRef c, PangoLayout *layout, PangoFontMetrics *metrics, int y) {
//    PangoRectangle inkRect, logicalRect;
//
//    dl_CGContextSaveGState(c);
//
//    auto line = pango_layout_get_line(layout, 0);
//    pango_layout_line_get_pixel_extents(line, &inkRect, &logicalRect);
//
//    auto useRect = logicalRect;
//    useRect.x = 20;
//    useRect.y = y; // + pango_font_metrics_get_descent(metrics) / PANGO_SCALE;
//
//    dl_CGContextClipToRect(c, {{(dl_CGFloat) useRect.x,     (dl_CGFloat) useRect.y},
//                            {(dl_CGFloat) useRect.width, (dl_CGFloat) useRect.height}});
//    auto grad = easyGrad(0.8, 0.8, 0.3, 1, 1, 0.5, 0.4, 1);
//    dl_CGContextDrawLinearGradient(c, grad, {(dl_CGFloat) useRect.x, (dl_CGFloat) useRect.y},
//                                {(dl_CGFloat) (useRect.x + useRect.width), (dl_CGFloat) (useRect.y + useRect.height)}, 0);
//
//    // dotted border
//    double dashes[2] = {2, 2};
//    cairo_set_dash(c->cr, dashes, 2, 0);
//    cairo_set_line_width(c->cr, 1);
//    cairo_rectangle(c->cr, useRect.x, useRect.y, useRect.width, useRect.height);
//    cairo_set_source_rgb(c->cr, 0, 0, 0);
//    cairo_stroke(c->cr);
//
//    dl_CGContextRestoreGState(c);
//}
//
//void pointAt(dl_CGContextRef c, int x, int y) {
//    cairo_arc(c->cr, x, y, 1, 0, 2 * M_PI);
//    cairo_set_source_rgb(c->cr, 1, 0, 0);
//    cairo_fill(c->cr);
//}
//
//OPENDL_API void CDECL dlFontPage01(dl_CGContextRef c, int width, int height) {
//    dl_CGContextSetRGBFillColor(c, 0.5, 0.5, 0.5, 1);
//    dl_CGContextFillRect(c, {{0,             0},
//                          {(float) width, (float) height}});
//    dl_CGContextScaleCTM(c, 3, 3);
//
//    const char *text = "Quartz♪❦♛あぎ";
//    int y = 20;
//
//    auto layout = pango_cairo_create_layout(c->cr);
//    auto pango_context = pango_layout_get_context(layout);
//    pango_cairo_context_set_resolution(pango_context, 72);
//
//    pango_layout_set_text(layout, text, -1);
//
//    auto arial12Desc = pango_font_description_from_string("Arial 12");
//    auto arial12Metrics = pango_context_get_metrics(pango_context, arial12Desc, pango_language_get_default());
//
//    pointAt(c, 20, y);
//    pango_layout_set_font_description(layout, arial12Desc);
//    gradientRectAround(c, layout, arial12Metrics, y);
//    cairo_move_to(c->cr, 20, y);
//    cairo_set_source_rgb(c->cr, 0, 0, 0);
//    pango_cairo_show_layout(c->cr, layout);
//
//    // larger lines
//    auto times40Desc = pango_font_description_from_string("Times 40");
//    auto times40Metrics = pango_context_get_metrics(pango_context, times40Desc, pango_language_get_default());
////    auto times40Ascent = pango_font_metrics_get_ascent(times40Metrics) / PANGO_SCALE;
//
//    pango_layout_set_font_description(layout, times40Desc);
//
//    // solid blue
//    y += 30;
//    pointAt(c, 20, y);
//    gradientRectAround(c, layout, times40Metrics, y);
//    cairo_move_to(c->cr, 20, y);
//    cairo_set_source_rgb(c->cr, 0, 0.3, 1.0);
//    pango_cairo_show_layout(c->cr, layout);
//
//    // w/ stroke below
//    cairo_set_line_width(c->cr, 0.5);
//
//    // stroked only
//    y += 60;
//    pointAt(c, 20, y);
//    gradientRectAround(c, layout, times40Metrics, y);
//    cairo_move_to(c->cr, 20, y);
//    cairo_set_source_rgb(c->cr, 0, 0.3, 1.0);
//    pango_cairo_layout_path(c->cr, layout);
//    cairo_stroke(c->cr);
//
//    // stroked and filled
//    y += 60;
//    pointAt(c, 20, y);
//    gradientRectAround(c, layout, times40Metrics, y);
//
//    cairo_move_to(c->cr, 20, y);
//    cairo_set_source_rgb(c->cr, 0, 0.3, 1.0);
//    pango_cairo_layout_path(c->cr, layout);
//    cairo_fill(c->cr);
//
//    cairo_move_to(c->cr, 20, y);
//    pango_cairo_layout_path(c->cr, layout);
//    cairo_set_source_rgb(c->cr, 0, 0, 0);
//    cairo_stroke(c->cr);
//
//    // baseline render
//    auto baseline = pango_layout_get_baseline(layout) / PANGO_SCALE;
//
//    y += 100;
//    pointAt(c, 20, y);
//
//    cairo_move_to(c->cr, 20, y - baseline);
//    cairo_set_source_rgb(c->cr, 0, 0.3, 1.0);
//    pango_cairo_layout_path(c->cr, layout);
//    cairo_fill(c->cr);
//
//    cairo_move_to(c->cr, 20, y - baseline);
//    pango_cairo_layout_path(c->cr, layout);
//    cairo_set_source_rgb(c->cr, 0, 0, 0);
//    cairo_set_line_width(c->cr, 0.5);
//    cairo_stroke(c->cr);
//
//    // end
//    g_object_unref(layout);
//    pango_font_description_free(times40Desc);
//    pango_font_description_free(arial12Desc);
//}

#define MATCH(SUBSTR) \
        strFind = SUBSTR; \
        start = text.find(strFind); \
        length = strFind.length();

#define APPLY(ATTR) \
        attr = ATTR; \
        attr->start_index = (guint)start; \
        attr->end_index = (guint)(attr->start_index + length); \
        pango_attr_list_insert(attrList, attr); \
        todestroy.push_back(attr);

#define EXAPPLY(ATTR) \
        attrManager.setAttr(ATTR, start, length); \
        todestroy.push_back(ATTR);
// removed two below lines: they should be happening elsewhere (applyRangeSpec callback in StringAttrManager)
//        ATTR->start_index = (guint)start; \
//        ATTR->end_index = (guint)(attr->start_index + length); \

#define MATCHAPPLY(SUBSTR, ATTR) \
        MATCH(SUBSTR); \
        APPLY(ATTR);

#define EXMATCHAPPLY(SUBSTR, ATTR) \
        MATCH(SUBSTR); \
        EXAPPLY(ATTR);


#include "pango-renderer/ExtendedAttrs.h"

static void render_layout(cairo_t *cr, PangoLayout *layout, RGBAColor *defaultColor) {

    double last_y = -1;
    int line_i = 0;
    auto iter2 = pango_layout_get_iter(layout);
    do {
        PangoRectangle line_exts, line_ink_exts;

        auto line = pango_layout_iter_get_line_readonly(iter2);
        pango_layout_iter_get_line_extents(iter2, &line_ink_exts, &line_exts);
        auto baseline = pango_units_to_double(pango_layout_iter_get_baseline(iter2));
        //printf("line iter, baseline %.2f\n", baseline);
        do {
            // runs
            auto run = pango_layout_iter_get_run_readonly(iter2);
            if (run) {
                PangoRectangle run_exts;
                pango_layout_iter_get_run_extents(iter2, nullptr, &run_exts);

                auto lang = pango_script_get_sample_language((PangoScript) run->item->analysis.script);
                auto metrics = pango_font_get_metrics(run->item->analysis.font, lang);

                RGBAColor *fgColor = nullptr;
                RGBAColor *bgColor = nullptr;
//                PangoColor *fgColor = nullptr;
//                PangoColor *bgColor = nullptr;
                RGBAColor *highColor = nullptr;

                IntWrapper *strikeCount = nullptr;
                RGBAColor *strikeColor = nullptr;

                IntWrapper *ulStyle = nullptr;
                RGBAColor *ulColor = nullptr;

                for (auto item = run->item->analysis.extra_attrs; item != NULL; item = item->next) {
                    auto attr = (PangoAttribute *) item->data;
                    switch (attr->klass->type) {
//                        case PANGO_ATTR_FOREGROUND:
//                            fgColor = &((PangoAttrColor *) attr)->color;
//                            break;
//                        case PANGO_ATTR_BACKGROUND:
//                            bgColor = &((PangoAttrColor *) attr)->color;
//                            break;
                        default:
                            if (attr->klass->type == ExtendedAttrs::pangoAttrType()) {
                                auto *extAttrs = (ExtendedAttrs *) attr;
                                fgColor = (RGBAColor *) extAttrs->getFormatFor(kCustomFormatForeground);
                                bgColor = (RGBAColor *) extAttrs->getFormatFor(kCustomFormatBackground);
                                highColor = (RGBAColor *) extAttrs->getFormatFor(kCustomFormatHighlight);
                                ulStyle = (IntWrapper *) extAttrs->getFormatFor(kCustomFormatUnderlineStyle);
                                ulColor = (RGBAColor *) extAttrs->getFormatFor(kCustomFormatUnderlineColor);
                                strikeCount = (IntWrapper *) extAttrs->getFormatFor(kCustomFormatStrikeCount);
                                strikeColor = (RGBAColor *) extAttrs->getFormatFor(kCustomFormatStrikeColor);
                            }
                            break;
                    }
                }

                auto x = pango_units_to_double(run_exts.x);
                auto y = pango_units_to_double(run_exts.y);
//                if (y > last_y) {
//                    printf("new line %d\n", line_i++);
//                    printf("==============\n");
//                    last_y = y;
//                }
//                printf(" - y: %.2f\n", y);

                auto width = pango_units_to_double(run_exts.width);
                auto height = pango_units_to_double(
                        line_exts.height); // use line height instead, won't change with font

                if (bgColor) {
                    bgColor->set_cairo_color(cr);
//                    cairo_set_source_rgb(cr, bgColor->red / 65535.0, bgColor->green / 65535.0, bgColor->blue / 65535.0);
                    cairo_rectangle(cr, x, y, width, height);
                    cairo_fill(cr);
                }

                if (ulStyle && ulStyle->value != UnderlineStyle::None) {
                    auto ul_pos = pango_units_to_double(
                            pango_font_metrics_get_underline_position(metrics));
                    auto ul_thickness = pango_units_to_double(
                            pango_font_metrics_get_underline_thickness(metrics));

                    auto ul_y = y + baseline - ul_pos;
                    cairo_set_line_width(cr, ul_thickness);

                    if (ulColor) {
                        ulColor->set_cairo_color(cr);
                    } else if (fgColor) {
                        fgColor->set_cairo_color(cr);
                    } else {
                        defaultColor->set_cairo_color(cr);
                    }

                    if (ulStyle->value >= UnderlineStyle::Single && ulStyle->value <= UnderlineStyle::Triple) {
                        cairo_move_to(cr, x, ul_y);
                        cairo_line_to(cr, x + width, ul_y);
                        cairo_stroke(cr);
                        if (ulStyle->value >= UnderlineStyle::Double) {
                            ul_y += ul_thickness * 2.0;
                            cairo_move_to(cr, x, ul_y);
                            cairo_line_to(cr, x + width, ul_y);
                            cairo_stroke(cr);
                            if (ulStyle->value == UnderlineStyle::Triple) {
                                ul_y += ul_thickness * 2.0;
                                cairo_move_to(cr, x, ul_y);
                                cairo_line_to(cr, x + width, ul_y);
                                cairo_stroke(cr);
                            }
                        }
                    } else if (ulStyle->value == UnderlineStyle::Squiggly) {
                        auto amplitude = ul_thickness;
                        auto period = ul_thickness * 5.0;

                        for (int t = 0; t < width; t++) {
                            auto ux = x + t;
                            auto angle = ((2.0f * M_PI * fmod(ux, period)) / period);
                            auto uy = ul_y + amplitude * sin(angle);

                            if (t == 0) {
                                cairo_move_to(cr, ux, uy);
                            } else {
                                cairo_line_to(cr, ux, uy);
                            }
                        }
                        cairo_stroke(cr);
                    } else if (ulStyle->value == UnderlineStyle::Overline) {
                        auto ink_y = pango_units_to_double(line_ink_exts.y);
                        auto ol_y = y + ink_y;
                        cairo_move_to(cr, x, ol_y);
                        cairo_line_to(cr, x + width, ol_y);
                        cairo_stroke(cr);
                    }
                }

                // draw glyphs
                if (fgColor) {
                    fgColor->set_cairo_color(cr);
//                    cairo_set_source_rgb(cr, fgColor->red / 65535.0, fgColor->green / 65535.0, fgColor->blue / 65535.0);
                } else {
                    defaultColor->set_cairo_color(cr);
                }
                //printf("y + baseline: %.2f\n", y + baseline);
                cairo_move_to(cr, x, y + baseline);
                pango_cairo_show_glyph_string(cr, run->item->analysis.font, run->glyphs);
                //pango_cairo_glyph_string_path ()
                //pango_cairo_layout_line_path() etc


                // strikethrough on top
                if (strikeCount) {
                    auto strike_pos = pango_units_to_double(
                            pango_font_metrics_get_strikethrough_position(metrics));
                    auto strike_thickness = pango_units_to_double(
                            pango_font_metrics_get_strikethrough_thickness(metrics));

                    cairo_set_line_width(cr, strike_thickness);

                    if (strikeColor) {
                        strikeColor->set_cairo_color(cr);
                    } else if (fgColor) {
                        fgColor->set_cairo_color(cr);
                    } else {
                        defaultColor->set_cairo_color(cr);
                    }

                    auto strike_y = y + baseline - strike_pos;

                    if (strikeCount->value == 1 || strikeCount->value == 3) {
                        // center line
                        cairo_move_to(cr, x, strike_y);
                        cairo_line_to(cr, x + width, strike_y);
                        cairo_stroke(cr);
                    }
                    if (strikeCount->value == 2 || strikeCount->value == 3) {
                        auto offset = strike_thickness * (strikeCount->value - 1); // 1 or 2

                        // two lines alone, or flanking a center line
                        auto top_y = strike_y - offset;
                        cairo_move_to(cr, x, top_y);
                        cairo_line_to(cr, x + width, top_y);
                        cairo_stroke(cr);

                        auto bottom_y = strike_y + offset;
                        cairo_move_to(cr, x, bottom_y);
                        cairo_line_to(cr, x + width, bottom_y);
                        cairo_stroke(cr);
                    }
                }

                // highlight on top
                if (highColor) {
                    highColor->set_cairo_color(cr);
                    cairo_rectangle(cr, x, y, width, height);
                    cairo_fill(cr);
                }

                pango_font_metrics_unref(metrics);
            } else {
                // null run = end of line?
            }
        } while (pango_layout_iter_next_run(iter2));
    } while (pango_layout_iter_next_line(iter2));
    pango_layout_iter_free(iter2);
}

OPENDL_API void CDECL dlFontPage02Old(dl_CGContextRef c, int width, int height) {

    RGBAColor black(0, 0, 0, 1);
    RGBAColor red(1, 0, 0, 1);
    RGBAColor green(0, 1, 0, 1);
    RGBAColor blue(0, 0, 1, 1);
    RGBAColor yellow(1, 1, 0, 1);
    RGBAColor alphaYellow(1, 1, 0, 0.5);
    RGBAColor magenta(1, 0, 1, 1);

    // woot
    dl_CGContextSetRGBFillColor(c, 0.39, 0.58, 0.92, 1);
    dl_CGContextFillRect(c, {{0,             0},
                          {(float) width, (float) height}});

    dl_CGContextSetTextMatrix(c, dl_CGAffineTransformIdentity);

    std::string text =
            "This paragraph of text rendered with DirectWrite is based on IDWriteTextFormat and IDWriteTextLayout objects"
            " and uses a custom format specifier passed to the SetDrawingEffect method, and thus is capable of different"
            " RBG RGB foreground colors, such as red, green, and blue as well as double underline, triple underline,"
            " single strikethrough, double strikethrough, triple strikethrough, or combinations thereof. Also possible is"
            " something often referred to as an overline, as well as a squiggly (squiggly?) underline.";

    auto cr = ((CGContextRef)c)->getCairoContext();

    auto layout = pango_cairo_create_layout(cr);
    auto pango_context = pango_layout_get_context(layout);
    auto fontMap = pango_cairo_font_map_get_default();
    //auto pango_context = pango_font_map_create_context(fontMap);
    //pango_cairo_context_set_resolution(pango_context, 72);

    pango_layout_set_text(layout, text.c_str(), -1);

    auto timesDesc = pango_font_description_from_string("Liberation Serif 40");

//    auto fmres = pango_cairo_font_map_get_resolution((PangoCairoFontMap *)fontMap);
//    printf("####fmres: %.2f\n", fmres);

    auto font = pango_font_map_load_font(fontMap, pango_context, timesDesc);
    auto timesMetrics = pango_context_get_metrics(pango_context, timesDesc, pango_language_get_default());

    pango_layout_set_font_description(layout, timesDesc);

    pango_layout_set_width(layout, pango_units_from_double(width));
    pango_layout_set_height(layout, pango_units_from_double(height));
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
    //pango_layout_set_justify(layout, TRUE);

    auto attrList = pango_attr_list_new();
    std::vector<PangoAttribute *> todestroy;

    PangoAttribute *attr;

    std::string strFind;
    unsigned int start, length;

    StringAttrManager attrManager;

    EXMATCHAPPLY("IDWriteTextFormat and IDWriteTextLayout objects",
                 ExtendedAttrs::createFormat(kCustomFormatBackground, &magenta));
    MATCHAPPLY("IDWriteTextFormat", pango_attr_style_new(PANGO_STYLE_ITALIC));
    MATCHAPPLY("IDWriteTextLayout", pango_attr_style_new(PANGO_STYLE_ITALIC));
    MATCHAPPLY("SetDrawingEffect", pango_attr_style_new(PANGO_STYLE_ITALIC));

    MATCH("RBG");
    auto single_strike = ExtendedAttrs::createFormat(kCustomFormatStrikeCount, new IntWrapper(1));
    EXAPPLY(single_strike);
    length = 1;
    EXAPPLY(ExtendedAttrs::createFormat(kCustomFormatForeground, &red));
    start += 1;
    EXAPPLY(ExtendedAttrs::createFormat(kCustomFormatForeground, &blue));
    start += 1;
    EXAPPLY(ExtendedAttrs::createFormat(kCustomFormatForeground, &green));

    MATCH("RGB");
    auto yellow_underline = ExtendedAttrs::createFormat(kCustomFormatUnderlineStyle,
                                                        new IntWrapper(UnderlineStyle::Single));
    yellow_underline->addFormat(kCustomFormatUnderlineColor, &yellow);
    EXAPPLY(yellow_underline);
    length = 1;
    EXAPPLY(ExtendedAttrs::createFormat(kCustomFormatForeground, &red));
    start += 1;
    EXAPPLY(ExtendedAttrs::createFormat(kCustomFormatForeground, &green));
    start += 1;
    EXAPPLY(ExtendedAttrs::createFormat(kCustomFormatForeground, &blue));

    EXMATCHAPPLY(" red", ExtendedAttrs::createFormat(kCustomFormatForeground, &red));
    EXMATCHAPPLY("green", ExtendedAttrs::createFormat(kCustomFormatForeground, &green));
    EXMATCHAPPLY("blue", ExtendedAttrs::createFormat(kCustomFormatForeground, &blue));

    MATCH("double underline");
    auto double_underline = ExtendedAttrs::createFormat(kCustomFormatUnderlineStyle,
                                                        new IntWrapper(UnderlineStyle::Double));
    double_underline->addFormat(kCustomFormatUnderlineColor, &red);
    EXAPPLY(double_underline);

    MATCH("triple underline");
    auto triple_underline = ExtendedAttrs::createFormat(kCustomFormatUnderlineStyle,
                                                        new IntWrapper(UnderlineStyle::Triple));
    triple_underline->addFormat(kCustomFormatUnderlineColor, &black);
    EXAPPLY(triple_underline);

    MATCH("single strikethrough");
    auto single_strike2 = ExtendedAttrs::createFormat(kCustomFormatStrikeCount, new IntWrapper(1));
    EXAPPLY(single_strike2);

    MATCH("double strikethrough");
    auto double_strike = ExtendedAttrs::createFormat(kCustomFormatStrikeCount, new IntWrapper(2));
    EXAPPLY(double_strike);

    MATCH("triple strikethrough");
    auto triple_strike = ExtendedAttrs::createFormat(kCustomFormatStrikeCount, new IntWrapper(3));
    EXAPPLY(triple_strike);

    MATCH("combinations");
    auto combs = ExtendedAttrs::createFormat(kCustomFormatStrikeCount, new IntWrapper(2));
    combs->addFormat(kCustomFormatStrikeColor, &red);
    combs->addFormat(kCustomFormatUnderlineStyle, new IntWrapper(UnderlineStyle::Triple));
    EXAPPLY(combs);

    MATCH("thereof");
    auto thereof = ExtendedAttrs::createFormat(kCustomFormatStrikeCount, new IntWrapper(3));
    thereof->addFormat(kCustomFormatUnderlineStyle, new IntWrapper(UnderlineStyle::Double));
    thereof->addFormat(kCustomFormatUnderlineColor, &blue);
    EXAPPLY(thereof);

    MATCH("squiggly (squiggly?) underline");
    auto squiggly_blue = ExtendedAttrs::createFormat(kCustomFormatUnderlineStyle,
                                                     new IntWrapper(UnderlineStyle::Squiggly));
    squiggly_blue->addFormat(kCustomFormatUnderlineColor, &blue);
    EXAPPLY(squiggly_blue);

    MATCH("(squiggly?)");
    auto squiggly_red = ExtendedAttrs::createFormat(kCustomFormatUnderlineColor, &red);
    EXAPPLY(squiggly_red);

    MATCH("overline");
    auto overline = ExtendedAttrs::createFormat(kCustomFormatUnderlineStyle, new IntWrapper(UnderlineStyle::Overline));
    overline->addFormat(kCustomFormatUnderlineColor, &blue);
    EXAPPLY(overline);

    auto highlight = ExtendedAttrs::createFormat(kCustomFormatHighlight, &alphaYellow);
    EXMATCHAPPLY("the SetDrawingEffect method", highlight);

    auto squiggly_high = ExtendedAttrs::createFormat(kCustomFormatHighlight, &alphaYellow);
    squiggly_high->addFormat(kCustomFormatUnderlineStyle, new IntWrapper(UnderlineStyle::Squiggly));
    squiggly_high->addFormat(kCustomFormatUnderlineColor, &blue);
    EXMATCHAPPLY("IDWriteTextLayout", squiggly_high);

    // sort out all the extended attributes (properly merges all the overlapping ones)
    attrManager.apply(attrList);

    // apply all of them, the regular and extended attributes
    pango_layout_set_attributes(layout, attrList);
    // release, layout should own the attrs now
    pango_attr_list_unref(attrList);

    // yay!
    render_layout(cr, layout, &black);

//    // release
//    pango_attr_list_unref(attrList);
//    for (auto i = todestroy.begin(); i < todestroy.end(); i++) {
//        pango_attribute_destroy(*i);
//    }
    g_object_unref(layout);
    g_object_unref(font);
//    g_object_unref(fontMap);
    pango_font_description_free(timesDesc);
}

//static void luminance_to_alpha(cairo_surface_t *surface, int width, int height) {
//    //16843009 (UINT_MAX / 255)
//    // * 0.2126 =  3580823.7134 (r coeff)
//    // * 0.7152 = 12046120.0368 (g coeff)
//    // * 0.0722 =  1216065.2498 (b coeff)
//    auto const r_coeff = (uint32_t) 3580823 + 1; // the +1 is to account for the roundoff error (lost fractional parts)
//    auto const g_coeff = (uint32_t) 12046120;
//    auto const b_coeff = (uint32_t) 1216065;
//
//    auto data = (uint32_t *) cairo_image_surface_get_data(surface);
//    for (int i = 0; i < width * height; i++) {
//        auto pixel = *data;
//        uint8_t r = pixel & 0xFF;
//        uint8_t g = (pixel & 0xFF00) >> 8;
//        uint8_t b = (pixel & 0xFF0000) >> 16;
//        uint32_t alpha = (r * r_coeff) + (g * g_coeff) + (b *
//                                                          b_coeff); // essentially a fixed point multiplication where we only care about the top 8 bits of the result
//        pixel &= 0x00FFFFFF; // clear any existing alpha on the target pixel
//        pixel |= alpha & 0xFF000000; // set pixel alpha to top 8 bits of calculated luminance
//        (*data++) = pixel;
//    }
//}
//
//OPENDL_API void CDECL dlFontPage03(dl_CGContextRef c, int width, int height) {
//    dl_CGContextSetRGBFillColor(c, 0, 0, 0, 1);
//    dl_CGContextFillRect(c, dl_CGRectMake(0, 0, width, height));
//
//    auto alpha_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
//    auto alpha_context = cairo_create(alpha_surface);
//    // clear alpha to black
//    cairo_set_source_rgb(alpha_context, 0, 0, 0);
//    cairo_paint(alpha_context);
//
//    // when API is told to apply mask,
//    // create offscreen buffer for all drawing until mask is released
//    // all drawing happens on that buffer and then is mask-applied to real context
//    // (buffer is cleared after every application)
//    auto buffer_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
//    auto buffer_context = cairo_create(buffer_surface);
//    auto bc = dl_CGContextCreateCairo(buffer_context, width, height);
//
//    // stroke text outline to mask surface
//    const char *text = "Quartz♪❦♛あぎ";
//    int y = 20;
//
//    auto layout = pango_cairo_create_layout(alpha_context);
//    auto pango_context = pango_layout_get_context(layout);
//    pango_cairo_context_set_resolution(pango_context, 72);
//
//    pango_layout_set_text(layout, text, -1);
//
//    auto fontDesc = pango_font_description_from_string("Arial 160");
////    auto fontMetrics = pango_context_get_metrics(pango_context, fontDesc, pango_language_get_default());
//
//    pango_layout_set_font_description(layout, fontDesc);
//    cairo_move_to(alpha_context, 20, y);
//
//    pango_cairo_layout_path(alpha_context, layout); // text outline to path
//
//    cairo_set_source_rgb(alpha_context, 1, 1, 1); // white
//    cairo_set_line_width(alpha_context, 8);
//    cairo_stroke(alpha_context);
//
//    // draw gradient to buffer
//    PangoRectangle inkRect, logicalRect;
//    auto line = pango_layout_get_line(layout, 0);
//    pango_layout_line_get_pixel_extents(line, &inkRect, &logicalRect);
//
//    dl_CGContextSaveGState(bc);
//
//    auto useRect = logicalRect;
//    printf("useRect: %d,%d,%d,%d\n", useRect.x, useRect.y, useRect.width, useRect.height);
//    useRect.x = 20;
//    useRect.y = y + 30; // + pango_font_metrics_get_descent(metrics) / PANGO_SCALE;
//    useRect.height -= 40;
//
//    // draw rect line for debugging
//    dl_CGContextSetRGBStrokeColor(c, 1, 0, 0, 1);
//    dl_CGContextSetLineWidth(c, 2);
//    dl_CGContextStrokeRect(c, dl_CGRectMake(useRect.x, useRect.y, useRect.width, useRect.height));
//
//
//    dl_CGContextClipToRect(bc, {{(dl_CGFloat) useRect.x,     (dl_CGFloat) useRect.y},
//                             {(dl_CGFloat) useRect.width, (dl_CGFloat) useRect.height}});
//    auto grad = easyGrad(0, 0, 0, 1, 0, 1, 1, 1);
//    auto start = dl_CGPointMake(useRect.x, useRect.y);
//    auto end = dl_CGPointMake(useRect.x, useRect.y + useRect.height);
//    dl_CGContextDrawLinearGradient(bc, grad, start, end, 0);
//
//    dl_CGContextRestoreGState(bc);
//    // buffer now contains raw gradient
//
//    // apply mask
//    cairo_set_source_surface(c->cr, buffer_surface, 0, 0); // implicitly creates pattern source
//    // convert luminance to alpha first, that's how cairo masks
//    luminance_to_alpha(alpha_surface, width, height);
//    cairo_mask_surface(c->cr, alpha_surface, 0, 0);
//
//    // clear buffer when done
//    dl_CGContextSetRGBFillColor(bc, 0, 0, 0, 1);
//    dl_CGContextFillRect(bc, dl_CGRectMake(0, 0, width, height));
//
//    // clear alpha buffer (luminance only)
//    cairo_set_source_rgb(alpha_context, 0, 0, 0); // black
//    cairo_paint(alpha_context);
////    cairo_set_operator(alpha_context, CAIRO_OPERATOR_CLEAR);
////    cairo_paint(alpha_context);
////    cairo_set_operator(alpha_context, CAIRO_OPERATOR_SOURCE);
//
//    // new path =====
//    cairo_move_to(alpha_context, 20, y);
//    pango_cairo_layout_path(alpha_context, layout);
//    cairo_set_source_rgb(alpha_context, 1, 1, 1); // white
//    cairo_fill(alpha_context);
//
//    // new gradient
//    line = pango_layout_get_line(layout, 0);
//    pango_layout_line_get_pixel_extents(line, &inkRect, &logicalRect);
//
//    dl_CGContextSaveGState(bc);
//
//    useRect = logicalRect;
//    useRect.x = 20;
//    useRect.y = y + 50; // + pango_font_metrics_get_descent(metrics) / PANGO_SCALE;
//
//    dl_CGContextClipToRect(bc, {{(dl_CGFloat) useRect.x,     (dl_CGFloat) useRect.y},
//                             {(dl_CGFloat) useRect.width, (dl_CGFloat) useRect.height}});
//    grad = easyGrad(1, 0.5, 0, 1, 0, 0, 0, 1);
//    start = dl_CGPointMake(useRect.x, useRect.y);
//    end = dl_CGPointMake(useRect.x, useRect.y + useRect.height);
//    dl_CGContextDrawLinearGradient(bc, grad, start, end, 0);
//
//    dl_CGContextRestoreGState(bc);
//    // buffer now contains raw gradient
//
//    // apply mask
//    cairo_set_source_surface(c->cr, buffer_surface, 0, 0); // implicitly creates pattern source
//    luminance_to_alpha(alpha_surface, width, height);
//    cairo_mask_surface(c->cr, alpha_surface, 0, 0);
//
//    // clear buffer when done
//    dl_CGContextSetRGBFillColor(bc, 0, 0, 0, 0);
//    dl_CGContextFillRect(bc, dl_CGRectMake(0, 0, width, height));
//
//    // clear alpha surface
//    cairo_set_source_rgb(alpha_context, 0, 0, 0);
//    cairo_paint(alpha_context);
//
//    // ========
//    g_object_unref(layout);
//    pango_font_description_free(fontDesc);
//
//    dl_CGContextDestroy(bc);
//    cairo_destroy(buffer_context);
//    cairo_surface_destroy(buffer_surface);
//
//    cairo_destroy(alpha_context);
//    cairo_surface_destroy(alpha_surface);
//}
