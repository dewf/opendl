#import "opendl.h"
#import <CoreGraphics/CoreGraphics.h>
//#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "../common/ExoClass.h"

#define UGLYCAST(x, y) *((x *)&y)
#define CC(x) (CGContextRef)x // Context Cast - save some typing since we have to cast all over the place from dlContext

// constant definitions ==========
const dl_CFIndex dl_kCFNotFound = kCFNotFound;
const dl_CFRange dl_CFRangeZero = dl_CFRangeMake(0, 0);
const dl_CGPoint dl_CGPointZero = UGLYCAST(dl_CGPoint, CGPointZero);
const dl_CGRect dl_CGRectZero = UGLYCAST(dl_CGRect, CGRectZero);
const dl_CGAffineTransform dl_CGAffineTransformIdentity = UGLYCAST(dl_CGAffineTransform, CGAffineTransformIdentity);
const dl_CFStringRef dl_kCTForegroundColorAttributeName = (dl_CFStringRef)kCTForegroundColorAttributeName;
const dl_CFStringRef dl_kCTForegroundColorFromContextAttributeName = (dl_CFStringRef)kCTForegroundColorFromContextAttributeName;
const dl_CFStringRef dl_kCTFontAttributeName = (dl_CFStringRef)kCTFontAttributeName;
const dl_CFStringRef dl_kCTParagraphStyleAttributeName = (dl_CFStringRef)kCTParagraphStyleAttributeName;
const dl_CFStringRef dl_kCTStrokeWidthAttributeName = (dl_CFStringRef)kCTStrokeWidthAttributeName;
const dl_CFStringRef dl_kCTStrokeColorAttributeName = (dl_CFStringRef)kCTStrokeColorAttributeName;

// struct-related methods
OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformTranslate(dl_CGAffineTransform t, dl_CGFloat tx, dl_CGFloat ty)
{
    auto ret = CGAffineTransformTranslate(UGLYCAST(CGAffineTransform, t), tx, ty);
    return UGLYCAST(dl_CGAffineTransform, ret);
}

OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformRotate(dl_CGAffineTransform t, dl_CGFloat angle)
{
    auto ret = CGAffineTransformRotate(UGLYCAST(CGAffineTransform, t), (CGFloat)angle);
    return UGLYCAST(dl_CGAffineTransform, ret);
}

OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformScale(dl_CGAffineTransform t, dl_CGFloat sx, dl_CGFloat sy)
{
    auto ret = CGAffineTransformScale(UGLYCAST(CGAffineTransform, t), sx, sy);
    return UGLYCAST(dl_CGAffineTransform, ret);
}

OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformConcat(dl_CGAffineTransform t1, dl_CGAffineTransform t2)
{
    auto ret = CGAffineTransformConcat(UGLYCAST(CGAffineTransform, t1), UGLYCAST(CGAffineTransform, t2));
    return UGLYCAST(dl_CGAffineTransform, ret);
}

OPENDL_API dl_CGPoint CDECL dl_CGPointApplyAffineTransform(dl_CGPoint p, dl_CGAffineTransform t)
{
    auto ret = CGPointApplyAffineTransform(UGLYCAST(CGPoint, p), UGLYCAST(CGAffineTransform, t));
    return UGLYCAST(dl_CGPoint, ret);
}

// API functions begin  ==========

OPENDL_API int CDECL dl_Init(struct dl_PlatformOptions *options)
{
    // nothing yet
    return 0;
}

OPENDL_API void CDECL dl_Shutdown()
{
    // nothing yet
}

struct ExoContext : ExoClass {
    // data to be associated with a dlContext (since this is easier than true CF subclassing)
    dl_CGContextRef context;
    int height;
    CGAffineTransform flipMatrix, inverseMatrix;
    ExoContext(dl_CGContextRef c, int height)
        :ExoClass(c),
         context(c),
         height(height)
    {
        auto m1 = CGAffineTransformTranslate(CGAffineTransformIdentity, 0, height);
        flipMatrix = CGAffineTransformScale(m1, 1.0, -1.0);
        inverseMatrix = CGAffineTransformInvert(flipMatrix);
    }
    ~ExoContext() {
        // nothing yet
        // de-association taken care of by dlExoClass
    }
    void beginFlip() {
        CGContextConcatCTM(CC(context), flipMatrix);
    }
    void endFlip() {
        // FYI, we can't do a Save/RestoreGState because doing a Restore pops the clipping mask! and sometimes we definitely don't want that!!! (sigh)
        // this isn't more difficult, but it was less obvious that the other way had unwanted side effects :)
        CGContextConcatCTM(CC(context), inverseMatrix);
    }
    CGPoint transformPoint(const CGPoint &p) {
        return CGPointApplyAffineTransform(p, flipMatrix);
    }
    static ExoContext *getFor(dl_CGContextRef c) {
        return (ExoContext *) ExoClass::getFor(c);
    }
};

struct BitmapExoContext : public ExoContext
{
    BitmapExoContext(dl_CGContextRef c, int height) : ExoContext(c, height) {}
    ~BitmapExoContext() override {
        // unlike the device context, the bitmap context owns the native CGContext
        CGContextRelease(CC(context));
    }
};

OPENDL_API dl_CGContextRef CDECL dl_CGContextCreateQuartz(CGContextRef context, int height)
{
    new ExoContext((dl_CGContextRef)context, height); // this auto-associates with the context
    return (dl_CGContextRef)context;
}

OPENDL_API void dl_CGContextRelease(dl_CGContextRef c)
{
    auto exoContext = ExoContext::getFor(c);
    delete exoContext; // de-associate
}

OPENDL_API void CDECL dl_CGContextSetRGBFillColor(dl_CGContextRef c, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha)
{
    CGContextSetRGBFillColor(CC(c), red, green, blue, alpha);
}

OPENDL_API void CDECL dl_CGContextFillRect(dl_CGContextRef c, dl_CGRect rect)
{
    CGContextFillRect(CC(c), UGLYCAST(CGRect, rect));
}

OPENDL_API void CDECL dl_CGContextSetRGBStrokeColor(dl_CGContextRef c, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha)
{
    CGContextSetRGBStrokeColor(CC(c), red, green, blue, alpha);
}

OPENDL_API void CDECL dl_CGContextStrokeRect(dl_CGContextRef c, dl_CGRect rect)
{
    CGContextStrokeRect(CC(c), UGLYCAST(CGRect, rect));
}

OPENDL_API void CDECL dl_CGContextStrokeRectWithWidth(dl_CGContextRef c, dl_CGRect rect, dl_CGFloat width)
{
    CGContextStrokeRectWithWidth(CC(c), UGLYCAST(CGRect, rect), width);
}

OPENDL_API void CDECL dl_CGContextBeginPath(dl_CGContextRef c)
{
    CGContextBeginPath(CC(c));
}

OPENDL_API void CDECL dl_CGContextClosePath(dl_CGContextRef c)
{
    CGContextClosePath(CC(c));
}

OPENDL_API void CDECL dl_CGContextMoveToPoint(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y)
{
    CGContextMoveToPoint(CC(c), x, y);
}

OPENDL_API void CDECL dl_CGContextAddLineToPoint(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y)
{
    CGContextAddLineToPoint(CC(c), x, y);
}

OPENDL_API void CDECL dl_CGContextAddRect(dl_CGContextRef c, dl_CGRect rect)
{
    CGContextAddRect(CC(c), UGLYCAST(CGRect, rect));
}

OPENDL_API void CDECL dl_CGContextAddArc(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, int clockwise)
{
    CGContextAddArc(CC(c), x, y, radius, startAngle, endAngle, clockwise);
}

OPENDL_API void CDECL dl_CGContextDrawPath(dl_CGContextRef c, dl_CGPathDrawingMode mode)
{
    CGContextDrawPath(CC(c), (CGPathDrawingMode)mode);
}

OPENDL_API void CDECL dl_CGContextStrokePath(dl_CGContextRef c)
{
    CGContextStrokePath(CC(c));
}

OPENDL_API void CDECL dl_CGContextSetLineWidth(dl_CGContextRef c, dl_CGFloat width)
{
    CGContextSetLineWidth(CC(c), width);
}

OPENDL_API void CDECL dl_CGContextSetLineDash(dl_CGContextRef c, dl_CGFloat phase, const dl_CGFloat *lengths, size_t count)
{
    CGContextSetLineDash(CC(c), phase, lengths, count);
}

OPENDL_API void CDECL dl_CGContextTranslateCTM(dl_CGContextRef c, dl_CGFloat tx, dl_CGFloat ty)
{
    CGContextTranslateCTM(CC(c), tx, ty);
}

OPENDL_API void CDECL dl_CGContextRotateCTM(dl_CGContextRef c, dl_CGFloat angle)
{
    CGContextRotateCTM(CC(c), angle);
}

OPENDL_API void CDECL dl_CGContextScaleCTM(dl_CGContextRef c, dl_CGFloat sx, dl_CGFloat sy)
{
    CGContextScaleCTM(CC(c), sx, sy);
}

OPENDL_API void CDECL dl_CGContextConcatCTM(dl_CGContextRef c, dl_CGAffineTransform transform)
{
    CGContextConcatCTM(CC(c), UGLYCAST(CGAffineTransform, transform));
}

OPENDL_API void CDECL dl_CGContextClip(dl_CGContextRef c)
{
    CGContextClip(CC(c));
}

OPENDL_API void CDECL dl_CGContextClipToRect(dl_CGContextRef c, dl_CGRect rect)
{
    CGContextClipToRect(CC(c), UGLYCAST(CGRect, rect));
}

OPENDL_API void dl_CGContextSaveGState(dl_CGContextRef c)
{
    CGContextSaveGState(CC(c));
}

OPENDL_API void dl_CGContextRestoreGState(dl_CGContextRef c)
{
    CGContextRestoreGState(CC(c));
}

/* path functions */

OPENDL_API dl_CGMutablePathRef dl_CGPathCreateMutable(void)
{
    return (dl_CGMutablePathRef)CGPathCreateMutable();
}

OPENDL_API void CDECL dl_CGPathAddRect(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGRect rect)
{
    CGPathAddRect((CGMutablePathRef)path, (const CGAffineTransform *)m, UGLYCAST(CGRect, rect));
}

OPENDL_API dl_CGPathRef dl_CGPathCreateWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform)
{
    return (dl_CGPathRef)CGPathCreateWithRect(UGLYCAST(CGRect, rect), (const CGAffineTransform *)transform);
}

OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform)
{
    return (dl_CGPathRef)CGPathCreateWithEllipseInRect(UGLYCAST(CGRect, rect), (const CGAffineTransform *)transform);
}

OPENDL_API void dl_CGPathRelease(dl_CGPathRef path)
{
    CGPathRelease((CGPathRef)path);
}

OPENDL_API void CDECL dl_CGContextAddPath(dl_CGContextRef context, dl_CGPathRef path)
{
    CGContextAddPath(CC(context), (CGPathRef)path);
}

// colorspace / gradient etc ====================

const dl_CFStringRef dl_kCGColorSpaceGenericGray = (dl_CFStringRef)kCGColorSpaceGenericGray;
const dl_CFStringRef dl_kCGColorSpaceGenericRGB = (dl_CFStringRef)kCGColorSpaceGenericRGB;
const dl_CFStringRef dl_kCGColorSpaceGenericCMYK = (dl_CFStringRef)kCGColorSpaceGenericCMYK;
const dl_CFStringRef dl_kCGColorSpaceGenericRGBLinear = (dl_CFStringRef)kCGColorSpaceGenericRGBLinear;
const dl_CFStringRef dl_kCGColorSpaceAdobeRGB1998 = (dl_CFStringRef)kCGColorSpaceAdobeRGB1998;
const dl_CFStringRef dl_kCGColorSpaceSRGB = (dl_CFStringRef)kCGColorSpaceSRGB;
const dl_CFStringRef dl_kCGColorSpaceGenericGrayGamma2_2 = (dl_CFStringRef)kCGColorSpaceGenericGrayGamma2_2;

OPENDL_API dl_CGColorSpaceRef CDECL dl_CGColorSpaceCreateWithName(dl_CFStringRef name)
{
    return (dl_CGColorSpaceRef)CGColorSpaceCreateWithName((CFStringRef)name);
}

OPENDL_API dl_CGColorSpaceRef CDECL dl_CGColorSpaceCreateDeviceGray(void)
{
    return (dl_CGColorSpaceRef)CGColorSpaceCreateDeviceGray();
}

OPENDL_API void CDECL dl_CGColorSpaceRelease(dl_CGColorSpaceRef colorSpace)
{
    CGColorSpaceRelease((CGColorSpaceRef)colorSpace);
}

OPENDL_API dl_CGColorRef dl_CGColorCreateGenericRGB(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha)
{
    return (dl_CGColorRef)CGColorCreateGenericRGB(red, green, blue, alpha);
}

OPENDL_API void dl_CGColorRelease(dl_CGColorRef color)
{
    CGColorRelease((CGColorRef)color);
}

OPENDL_API void CDECL dl_CGContextSetFillColorWithColor(dl_CGContextRef c, dl_CGColorRef color)
{
    CGContextSetFillColorWithColor(CC(c), (CGColorRef)color);
}

OPENDL_API void CDECL dl_CGContextSetStrokeColorWithColor(dl_CGContextRef c, dl_CGColorRef color)
{
    CGContextSetStrokeColorWithColor(CC(c), (CGColorRef)color);
}

const dl_CFStringRef dl_kCGColorWhite = (dl_CFStringRef)kCGColorWhite;
const dl_CFStringRef dl_kCGColorBlack = (dl_CFStringRef)kCGColorBlack;
const dl_CFStringRef dl_kCGColorClear = (dl_CFStringRef)kCGColorClear;

OPENDL_API dl_CGColorRef CDECL dl_CGColorGetConstantColor(dl_CFStringRef colorName)
{
    return (dl_CGColorRef)CGColorGetConstantColor((CFStringRef)colorName);
}

OPENDL_API size_t CDECL dl_CGColorGetNumberOfComponents(dl_CGColorRef color)
{
    return CGColorGetNumberOfComponents((CGColorRef)color);
}

OPENDL_API const CGFloat* CDECL dl_CGColorGetComponents(dl_CGColorRef color)
{
    return CGColorGetComponents((CGColorRef)color);
}

// gradients etc ================================

OPENDL_API dl_CGGradientRef CDECL dl_CGGradientCreateWithColorComponents(dl_CGColorSpaceRef space, const dl_CGFloat components[], const dl_CGFloat locations[], size_t count)
{
    return (dl_CGGradientRef)CGGradientCreateWithColorComponents((CGColorSpaceRef)space, components, locations, count);
}

OPENDL_API void CDECL dl_CGGradientRelease(dl_CGGradientRef gradient)
{
    CGGradientRelease((CGGradientRef)gradient);
}

OPENDL_API void CDECL dl_CGContextDrawLinearGradient(dl_CGContextRef c, dl_CGGradientRef gradient, dl_CGPoint startPoint, dl_CGPoint endPoint, dl_CGGradientDrawingOptions options)
{
    CGContextDrawLinearGradient(CC(c), (CGGradientRef)gradient, UGLYCAST(CGPoint, startPoint), UGLYCAST(CGPoint, endPoint), options);
}

// text =========================================

OPENDL_API void dl_CGContextSetTextMatrix(dl_CGContextRef c, dl_CGAffineTransform t)
{
    CGContextSetTextMatrix(CC(c), UGLYCAST(CGAffineTransform, t));
}

OPENDL_API dl_CTFontRef dl_CTFontCreateWithName(dl_CFStringRef name, dl_CGFloat size, const dl_CGAffineTransform *matrix)
{
    return (dl_CTFontRef)CTFontCreateWithName((CFStringRef)name, (CGFloat)size, (const CGAffineTransform *)matrix);
}

OPENDL_API dl_CFArrayRef CDECL dl_CTFontManagerCreateFontDescriptorsFromURL(dl_CFURLRef fileURL)
{
    return (dl_CFArrayRef)CTFontManagerCreateFontDescriptorsFromURL((CFURLRef)fileURL);
}

OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateWithFontDescriptor(dl_CTFontDescriptorRef descriptor, dl_CGFloat size, const dl_CGAffineTransform *matrix)
{
    return (dl_CTFontRef)CTFontCreateWithFontDescriptor((CTFontDescriptorRef)descriptor, size, (const CGAffineTransform *)matrix);
}

OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateCopyWithSymbolicTraits(dl_CTFontRef font, dl_CGFloat size, const dl_CGAffineTransform *matrix, dl_CTFontSymbolicTraits symTraitValue, dl_CTFontSymbolicTraits symTraitMask)
{
    return (dl_CTFontRef)CTFontCreateCopyWithSymbolicTraits((CTFontRef)font, size, (const CGAffineTransform *)matrix, (CTFontSymbolicTraits)symTraitValue, (CTFontSymbolicTraits)symTraitMask);
}

struct ExoFramesetter : ExoClass {
    dl_CTFramesetterRef fs;
    CGAffineTransform flipMatrix;
    ExoFramesetter(dl_CTFramesetterRef fs, int viewHeight)
        :ExoClass(fs), fs(fs)
    {
        auto m1 = CGAffineTransformTranslate(CGAffineTransformIdentity, 0, viewHeight);
        flipMatrix = CGAffineTransformScale(m1, 1.0, -1.0);
    }
    ~ExoFramesetter() override {
        // nothing yet
    }
    CGPathRef flippedPath(CGPathRef path) {
        return CGPathCreateCopyByTransformingPath(path, &flipMatrix);
    }
    static ExoFramesetter *getFor(dl_CTFramesetterRef fs) {
        return (ExoFramesetter *)ExoClass::getFor(fs);
    }
};

OPENDL_API dl_CTFramesetterRef dl_CTFramesetterCreateWithAttributedString(dl_CFAttributedStringRef string, int viewHeight)
{
    auto fs = (dl_CTFramesetterRef)CTFramesetterCreateWithAttributedString((CFAttributedStringRef)string);
    new ExoFramesetter(fs, viewHeight); // create + associate exoclass data
    return fs;
}

struct ExoFrame : ExoClass {
    dl_CTFrameRef frame;
    ExoFramesetter *framesetter; // our link back to context
    dl_CFIndex lineCount = 0;
    CGRect boundingRect;
    // ========================
    ExoFrame(dl_CTFrameRef frame, dl_CTFramesetterRef fs, CGRect boundingRect)
        :ExoClass(frame),
         frame(frame),
         framesetter(ExoFramesetter::getFor(fs)),
         boundingRect(boundingRect)
    {
        // nothing yet
    }
    ~ExoFrame() override {
        // nothing yet
    }
//    void flipPoint(CGPoint *p) {
//        *p = CGPointApplyAffineTransform(*p, framesetter->exoContext->flipMatrix);
//    }
    static ExoFrame *getFor(dl_CTFrameRef frame) {
        return (ExoFrame *)ExoClass::getFor(frame);
    }
};

OPENDL_API dl_CTFrameRef dl_CTFramesetterCreateFrame(dl_CTFramesetterRef framesetter,
                                               dl_CFRange stringRange, dl_CGPathRef path, dl_CFDictionaryRef frameAttributes)
{
    // before flipping, get the bounding box of the original path
    // needed for fixing up line origins (since they are all relative to it)
    auto bbRect = CGPathGetBoundingBox((CGPathRef)path);
    
    auto exoFS = ExoFramesetter::getFor(framesetter);
    auto flippedPath = exoFS->flippedPath((CGPathRef)path);

    auto frame = (dl_CTFrameRef)CTFramesetterCreateFrame((CTFramesetterRef)framesetter,
                                                UGLYCAST(CFRange, stringRange),
                                                (CGPathRef)flippedPath,
                                                (CFDictionaryRef)frameAttributes);
    
    new ExoFrame(frame, framesetter, bbRect); // create + associate ExoFrame data
    
    CGPathRelease(flippedPath);
    return frame;
}

OPENDL_API dl_CFArrayRef dl_CTFrameGetLines(dl_CTFrameRef frame)
{
    auto ret = CTFrameGetLines((CTFrameRef)frame);

    // save the number of lines in the ExoFrame, in case we need it later (eg, if GetLineOrigins called with a 0-length range)
    auto exoFrame = ExoFrame::getFor(frame);
    exoFrame->lineCount = (dl_CFIndex)CFArrayGetCount(ret);
    
    return (dl_CFArrayRef)ret;
}

OPENDL_API void dl_CTFrameGetLineOrigins(dl_CTFrameRef frame, dl_CFRange range, dl_CGPoint origins[])
{
    CTFrameGetLineOrigins((CTFrameRef)frame, UGLYCAST(CFRange, range), (CGPoint *)origins);
    // fixup y coordinates for top-down text rendering
    auto exoFrame = ExoFrame::getFor(frame);
    if (range.length == 0) {
        // caller wants all (remaining) lines - try to figure out how many that is
        range.length = exoFrame->lineCount - range.location;
        if (range.length <= 0) {
            printf("dl_CTFrameGetLineOrigins: bad range, be sure to call dl_CTFrameGetLines() before this (and/or check your math)\n");
            return;
        }
    }
    for (dl_CFIndex i = 0; i< range.length; i++) {
        auto p = &((CGPoint *)origins)[i];
        auto &r = exoFrame->boundingRect;
        p->x += r.origin.x;
        p->y = r.origin.y + (r.size.height - p->y);
    }
}

OPENDL_API double CDECL dl_CTLineGetTypographicBounds(dl_CTLineRef line, dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *leading)
{
    return CTLineGetTypographicBounds((CTLineRef)line, (CGFloat *)ascent, (CGFloat *)descent, (CGFloat *)leading);
}

OPENDL_API dl_CFArrayRef CDECL dl_CTLineGetGlyphRuns(dl_CTLineRef line)
{
    return (dl_CFArrayRef)CTLineGetGlyphRuns((CTLineRef)line);
}

OPENDL_API dl_CFDictionaryRef CDECL dl_CTRunGetAttributes(dl_CTRunRef run)
{
    return (dl_CFDictionaryRef)CTRunGetAttributes((CTRunRef)run);
}

OPENDL_API double CDECL dl_CTRunGetTypographicBounds(dl_CTRunRef run, dl_CFRange range, dl_CGFloat* ascent, dl_CGFloat* descent, dl_CGFloat* leading)
{
    return CTRunGetTypographicBounds((CTRunRef)run, UGLYCAST(CFRange, range), (CGFloat *)ascent, (CGFloat *)descent, (CGFloat *)leading);
}

OPENDL_API dl_CFRange CDECL dl_CTRunGetStringRange(dl_CTRunRef run)
{
    auto ret = CTRunGetStringRange((CTRunRef)run);
    return UGLYCAST(dl_CFRange, ret);
}

OPENDL_API dl_CTRunStatus CDECL dl_CTRunGetStatus(dl_CTRunRef run)
{
    return (dl_CTRunStatus)CTRunGetStatus((CTRunRef)run);
}

OPENDL_API dl_CGFloat CDECL dl_CTLineGetOffsetForStringIndex(dl_CTLineRef line, dl_CFIndex charIndex, dl_CGFloat* secondaryOffset)
{
    return CTLineGetOffsetForStringIndex((CTLineRef)line, (CFIndex)charIndex, (CGFloat *)secondaryOffset);
}

OPENDL_API dl_CFIndex CDECL dl_CTLineGetStringIndexForPosition(dl_CTLineRef line, dl_CGPoint position)
{
    return CTLineGetStringIndexForPosition((CTLineRef)line, UGLYCAST(CGPoint, position));
}

OPENDL_API dl_CFRange CDECL dl_CTLineGetStringRange(dl_CTLineRef line)
{
    auto ret = CTLineGetStringRange((CTLineRef)line);
    return UGLYCAST(dl_CFRange, ret);
}

OPENDL_API dl_CGFloat CDECL dl_CTFontGetUnderlineThickness(dl_CTFontRef font)
{
    return CTFontGetUnderlineThickness((CTFontRef)font);
}

OPENDL_API dl_CGFloat CDECL dl_CTFontGetUnderlinePosition(dl_CTFontRef font)
{
    return CTFontGetUnderlinePosition((CTFontRef)font) * -1.0; // negative since we're trying to do everything top-down, and Core Text is bottom-up
}

OPENDL_API void CDECL dl_CTFrameDraw(dl_CTFrameRef frame, dl_CGContextRef context)
{
    auto exoContext = ExoContext::getFor(context);
    exoContext->beginFlip();
    CTFrameDraw((CTFrameRef)frame, CC(context));
    exoContext->endFlip();
}

OPENDL_API void CDECL dl_CGContextSetTextPosition(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y)
{
    // fix y
    auto exoContext = ExoContext::getFor(c);
    auto flipped = exoContext->transformPoint(CGPointMake(x, y));
    CGContextSetTextPosition(CC(c), (CGFloat)flipped.x, (CGFloat)flipped.y);
}

OPENDL_API void CDECL dl_CTLineDraw(dl_CTLineRef line, dl_CGContextRef context)
{
    auto exoContext = ExoContext::getFor(context);
    exoContext->beginFlip();
    CTLineDraw((CTLineRef)line, CC(context));
    exoContext->endFlip();
}

OPENDL_API void CDECL dl_CGContextSetTextDrawingMode(dl_CGContextRef c, dl_CGTextDrawingMode mode)
{
    CGContextSetTextDrawingMode(CC(c), (CGTextDrawingMode)mode);
}

OPENDL_API dl_CTLineRef CDECL dl_CTLineCreateWithAttributedString(dl_CFAttributedStringRef string)
{
    return (dl_CTLineRef)CTLineCreateWithAttributedString((CFAttributedStringRef)string);
}

OPENDL_API dl_CGRect CDECL dl_CTLineGetBoundsWithOptions(dl_CTLineRef line, dl_CTLineBoundsOptions options)
{
    auto ret = CTLineGetBoundsWithOptions((CTLineRef)line, (CTLineBoundsOptions)options);

    // flip Y axis like everything else
    // y coord represents origin of bounding box relative to baseline
    ret.origin.y = -(ret.size.height - (-ret.origin.y));
    return UGLYCAST(dl_CGRect, ret);
}

OPENDL_API dl_CTParagraphStyleRef CDECL dl_CTParagraphStyleCreate(const dl_CTParagraphStyleSetting * settings,
                                                            size_t settingCount)
{
    return (dl_CTParagraphStyleRef)CTParagraphStyleCreate((const CTParagraphStyleSetting *)settings, settingCount);
}

// bitmaps ======================================

OPENDL_API dl_CGContextRef CDECL dl_CGBitmapContextCreate(void *data, size_t width,
                                                 size_t height, size_t bitsPerComponent, size_t bytesPerRow,
                                                 dl_CGColorSpaceRef space, dl_CGBitmapInfo bitmapInfo)
{
    auto ret = (dl_CGContextRef)CGBitmapContextCreate(data, width, height,
                                            bitsPerComponent, bytesPerRow, (CGColorSpaceRef)space, (CGBitmapInfo)bitmapInfo);
    new BitmapExoContext(ret, (int)height); // create + associate
    return ret;
}

OPENDL_API void * CDECL dl_CGBitmapContextGetData(dl_CGContextRef bitmap)
{
    return CGBitmapContextGetData((CGContextRef)bitmap);
}

OPENDL_API void CDECL dl_CGBitmapContextReleaseData(dl_CGContextRef bitmap)
{
    // no-op on Mac (see note in opendl.h)
}

OPENDL_API dl_CGImageRef CDECL dl_CGBitmapContextCreateImage(dl_CGContextRef context)
{
    return (dl_CGImageRef)CGBitmapContextCreateImage(CC(context));
}

OPENDL_API void dl_CGImageRelease(dl_CGImageRef image)
{
    CGImageRelease((CGImageRef)image);
}

OPENDL_API void CDECL dl_CGContextClipToMask(dl_CGContextRef c, dl_CGRect rect, dl_CGImageRef mask)
{
    CGContextClipToMask(CC(c), UGLYCAST(CGRect, rect), (CGImageRef)mask);
}

OPENDL_API void CDECL dl_CGContextDrawImage(dl_CGContextRef c, dl_CGRect rect, dl_CGImageRef image)
{
    CGContextDrawImage(CC(c), UGLYCAST(CGRect, rect), (CGImageRef)image);
}

// testing etc ==================================

//static dl_CGGradientRef getGradient(CGFloat fromR, CGFloat fromG, CGFloat fromB, CGFloat fromA,
//                          CGFloat toR, CGFloat toG, CGFloat toB, CGFloat toA) {
//    CGFloat locations[2] = { 0, 1 };
//    CGFloat components[8] = {
//        fromR, fromG, fromB, fromA,
//        toR, toG, toB, toA
//    };
//    auto colorspace = dl_CGColorSpaceCreateWithName(dl_kCGColorSpaceGenericRGB);
//    auto grad = dl_CGGradientCreateWithColorComponents(colorspace, components, locations, 2);
//    dl_CGColorSpaceRelease(colorspace);
//    return grad;
//}

OPENDL_API void CDECL dl_InternalTesting(dl_CGContextRef c, int width, int height)
{
    //
}

































