#ifndef OPENDL_LIBRARY_H
#define OPENDL_LIBRARY_H

#include "prologue.h" // buncha preprocessor nastiness

#ifdef DL_PLATFORM_WINDOWS
#	include <Windows.h>
#   include <d2d1.h>
#   include <d2d1_1.h>
#elif defined DL_PLATFORM_LINUX
#   include <cairo/cairo.h>
#elif defined DL_PLATFORM_MACOS
#   include <CoreGraphics/CoreGraphics.h>
#endif

#include "dlcf.h" // minimal core foundation implementation, required for many OpenDL calls

#ifdef __cplusplus
extern "C" {
#endif
    
    // API proper begin
	struct dl_PlatformOptions {
#ifdef DL_PLATFORM_WINDOWS
		ID2D1Factory *factory;
#else
		int reserved;
#endif
	};
	OPENDL_API int CDECL dl_Init(struct dl_PlatformOptions *options);
	OPENDL_API void CDECL dl_Shutdown();

#ifdef DL_PLATFORM_MACOS
    typedef CGFloat dl_CGFloat;
#else
    typedef double dl_CGFloat;
#endif
    
    typedef struct {
        dl_CGFloat x;
        dl_CGFloat y;
    } dl_CGPoint;
    OPENDL_API extern const dl_CGPoint dl_CGPointZero;
    OPENDL_API inline dl_CGPoint CDECL dl_CGPointMake(dl_CGFloat x, dl_CGFloat y) {
		dl_CGPoint ret;
		ret.x = x;
		ret.y = y;
		return ret;
    }
    
    typedef struct {
        dl_CGFloat width;
        dl_CGFloat height;
    } dl_CGSize;
    
    typedef struct {
        dl_CGFloat dx;
        dl_CGFloat dy;
    } dl_CGVector;
    
    typedef struct {
        dl_CGPoint origin;
        dl_CGSize size;
    } dl_CGRect;
	inline OPENDL_API dl_CGRect CDECL dl_CGRectMake(dl_CGFloat x, dl_CGFloat y, dl_CGFloat width, dl_CGFloat height) {
		dl_CGRect ret;
		ret.origin.x = x;
		ret.origin.y = y;
		ret.size.width = width;
		ret.size.height = height;
		return ret;
	}
	inline OPENDL_API dl_CGFloat CDECL dl_CGRectGetWidth(dl_CGRect rect) {
		return rect.size.width;
	}
    OPENDL_API extern const dl_CGRect dl_CGRectZero;
    
    typedef struct {
        dl_CGFloat a, b, c, d;
        dl_CGFloat tx, ty;
    } dl_CGAffineTransform;
    OPENDL_API extern const dl_CGAffineTransform dl_CGAffineTransformIdentity;
    OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformTranslate(dl_CGAffineTransform t, dl_CGFloat tx, dl_CGFloat ty);
    OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformRotate(dl_CGAffineTransform t, dl_CGFloat angle);
    OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformScale(dl_CGAffineTransform t, dl_CGFloat sx, dl_CGFloat sy);
    OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformConcat(dl_CGAffineTransform t1, dl_CGAffineTransform t2);
    OPENDL_API dl_CGAffineTransform CDECL dl_CGAffineTransformInvert(dl_CGAffineTransform t);
    OPENDL_API dl_CGPoint CDECL dl_CGPointApplyAffineTransform(dl_CGPoint p, dl_CGAffineTransform t);
    
    typedef enum  {
        dl_kCGPathFill,
        dl_kCGPathEOFill,
        dl_kCGPathStroke,
        dl_kCGPathFillStroke,
        dl_kCGPathEOFillStroke
    } dl_CGPathDrawingMode;

    DLHANDLE(CG,Context);

#ifdef DL_PLATFORM_WINDOWS
    OPENDL_API dl_CGContextRef CDECL dl_CGContextCreateD2D(ID2D1RenderTarget *target);
	OPENDL_API void CDECL dl_D2DTargetRecreated(ID2D1RenderTarget *newTarget, ID2D1RenderTarget *oldTarget);
#elif defined DL_PLATFORM_MACOS
    OPENDL_API dl_CGContextRef CDECL dl_CGContextCreateQuartz(CGContextRef context, int height); // need height for text flipping (Core Text doesn't know about isFlipped)
#elif defined DL_PLATFORM_LINUX
    OPENDL_API dl_CGContextRef CDECL dl_CGContextCreateCairo(cairo_t *cr, int width, int height); // need some extra info for various calls
#endif
	OPENDL_API void CDECL dl_CGContextRelease(dl_CGContextRef c);
    
    OPENDL_API void CDECL dl_CGContextSetRGBFillColor(dl_CGContextRef c, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha);
    OPENDL_API void CDECL dl_CGContextFillRect(dl_CGContextRef c, dl_CGRect rect);
    OPENDL_API void CDECL dl_CGContextSetRGBStrokeColor(dl_CGContextRef c, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha);
    OPENDL_API void CDECL dl_CGContextStrokeRect(dl_CGContextRef c, dl_CGRect rect);
    OPENDL_API void CDECL dl_CGContextStrokeRectWithWidth(dl_CGContextRef c, dl_CGRect rect, dl_CGFloat width);
    
    OPENDL_API void CDECL dl_CGContextBeginPath(dl_CGContextRef c);
    OPENDL_API void CDECL dl_CGContextClosePath(dl_CGContextRef c);
    OPENDL_API void CDECL dl_CGContextMoveToPoint(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y);
    OPENDL_API void CDECL dl_CGContextAddLineToPoint(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y);
    OPENDL_API void CDECL dl_CGContextAddRect(dl_CGContextRef c, dl_CGRect rect);
    OPENDL_API void CDECL dl_CGContextAddArc(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, int clockwise);
    OPENDL_API void CDECL dl_CGContextAddArcToPoint(dl_CGContextRef c, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius);
    OPENDL_API void CDECL dl_CGContextDrawPath(dl_CGContextRef c, dl_CGPathDrawingMode mode);
    OPENDL_API void CDECL dl_CGContextStrokePath(dl_CGContextRef c);
    OPENDL_API void CDECL dl_CGContextFillPath(dl_CGContextRef c);

    OPENDL_API void CDECL dl_CGContextSetLineWidth(dl_CGContextRef c, dl_CGFloat width);
    OPENDL_API void CDECL dl_CGContextSetLineDash(dl_CGContextRef c, dl_CGFloat phase, const dl_CGFloat *lengths, size_t count);
    
    OPENDL_API void CDECL dl_CGContextTranslateCTM(dl_CGContextRef c, dl_CGFloat tx, dl_CGFloat ty);
    OPENDL_API void CDECL dl_CGContextRotateCTM(dl_CGContextRef c, dl_CGFloat angle);
    OPENDL_API void CDECL dl_CGContextScaleCTM(dl_CGContextRef c, dl_CGFloat sx, dl_CGFloat sy);
    OPENDL_API void CDECL dl_CGContextConcatCTM(dl_CGContextRef c, dl_CGAffineTransform transform);
    
    OPENDL_API void CDECL dl_CGContextClip(dl_CGContextRef c);
    OPENDL_API void CDECL dl_CGContextClipToRect(dl_CGContextRef c, dl_CGRect rect);
    
    OPENDL_API void CDECL dl_CGContextSaveGState(dl_CGContextRef c);
    OPENDL_API void CDECL dl_CGContextRestoreGState(dl_CGContextRef c);
    
    // path stuff ============
	DLHANDLE_WITH_MUTABLE(CG,Path);

	OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform);
	OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform);
	OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateWithRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform);

	OPENDL_API dl_CGMutablePathRef CDECL dl_CGPathCreateMutable(void);
	OPENDL_API dl_CGPathRef CDECL dl_CGPathCreateCopy(dl_CGPathRef path);
	OPENDL_API dl_CGMutablePathRef CDECL dl_CGPathCreateMutableCopy(dl_CGPathRef path);

    OPENDL_API void CDECL dl_CGPathMoveToPoint(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y);
	OPENDL_API void CDECL dl_CGPathAddArc(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise);
	OPENDL_API void CDECL dl_CGPathAddRelativeArc(dl_CGMutablePathRef path, const dl_CGAffineTransform *matrix, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta);
	OPENDL_API void CDECL dl_CGPathAddArcToPoint(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius);
	OPENDL_API void CDECL dl_CGPathAddCurveToPoint(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y);
	OPENDL_API void CDECL dl_CGPathAddLines(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, const dl_CGPoint *points, size_t count);
	OPENDL_API void CDECL dl_CGPathAddLineToPoint(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y);
	OPENDL_API void CDECL dl_CGPathAddPath(dl_CGMutablePathRef path1, const dl_CGAffineTransform *m, dl_CGPathRef path2);
	OPENDL_API void CDECL dl_CGPathAddQuadCurveToPoint(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y);
	OPENDL_API void CDECL dl_CGPathAddRect(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGRect rect);
	OPENDL_API void CDECL dl_CGPathAddRects(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, const dl_CGRect *rects, size_t count);
	OPENDL_API void CDECL dl_CGPathAddRoundedRect(dl_CGMutablePathRef path, const dl_CGAffineTransform *transform, dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight);
	OPENDL_API void CDECL dl_CGPathAddEllipseInRect(dl_CGMutablePathRef path, const dl_CGAffineTransform *m, dl_CGRect rect);
    OPENDL_API void CDECL dl_CGPathCloseSubpath(dl_CGMutablePathRef path);

    OPENDL_API dl_CGPathRef CDECL dl_CGPathRetain(dl_CGPathRef path);
    OPENDL_API void CDECL dl_CGPathRelease(dl_CGPathRef path);

    OPENDL_API dl_CGPoint CDECL dl_CGPathGetCurrentPoint(dl_CGPathRef path);

    OPENDL_API void CDECL dl_CGContextAddPath(dl_CGContextRef context, dl_CGPathRef path);

	// colorspace etc ========
	DLHANDLE(CG,ColorSpace);

    OPENDL_API extern const dl_CFStringRef dl_kCGColorSpaceGenericGray;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorSpaceGenericRGB;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorSpaceGenericCMYK;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorSpaceGenericRGBLinear;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorSpaceAdobeRGB1998;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorSpaceSRGB;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorSpaceGenericGrayGamma2_2;
	OPENDL_API dl_CGColorSpaceRef CDECL dl_CGColorSpaceCreateWithName(dl_CFStringRef name);
    OPENDL_API dl_CGColorSpaceRef CDECL dl_CGColorSpaceCreateDeviceGray(void);
    
    OPENDL_API void CDECL dl_CGColorSpaceRelease(dl_CGColorSpaceRef colorSpace);

    DLHANDLE(CG,Color);
    OPENDL_API dl_CGColorRef CDECL dl_CGColorCreateGenericRGB(dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha);
    OPENDL_API void CDECL dl_CGColorRelease(dl_CGColorRef color);
    
    OPENDL_API void CDECL dl_CGContextSetFillColorWithColor(dl_CGContextRef c, dl_CGColorRef color);
    OPENDL_API void CDECL dl_CGContextSetStrokeColorWithColor(dl_CGContextRef c, dl_CGColorRef color);
    
	OPENDL_API extern const dl_CFStringRef dl_kCGColorWhite;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorBlack;
	OPENDL_API extern const dl_CFStringRef dl_kCGColorClear;
    OPENDL_API dl_CGColorRef CDECL dl_CGColorGetConstantColor(dl_CFStringRef colorName);
    
    OPENDL_API size_t CDECL dl_CGColorGetNumberOfComponents(dl_CGColorRef color);
    OPENDL_API const dl_CGFloat* CDECL dl_CGColorGetComponents(dl_CGColorRef color);

    // gradients =============
	DLHANDLE(CG,Gradient);
	OPENDL_API dl_CGGradientRef CDECL dl_CGGradientCreateWithColorComponents(dl_CGColorSpaceRef space, const dl_CGFloat components[], const dl_CGFloat locations[], size_t count);
    OPENDL_API void CDECL dl_CGGradientRelease(dl_CGGradientRef gradient);

	enum {
		dl_kCGGradientDrawsBeforeStartLocation = (1 << 0),
		dl_kCGGradientDrawsAfterEndLocation = (1 << 1),
	}; //dl_CGGradientDrawingOptions
	typedef unsigned int dl_CGGradientDrawingOptions;
	OPENDL_API void CDECL dl_CGContextDrawLinearGradient(dl_CGContextRef c, dl_CGGradientRef gradient, dl_CGPoint startPoint, dl_CGPoint endPoint, dl_CGGradientDrawingOptions options);

    // text ==================
    DLHANDLE(CT,Font);
	DLHANDLE(CT,FontDescriptor);
    DLHANDLE(CT,Framesetter);
    DLHANDLE(CT,Frame);
    DLHANDLE(CT,Line);
    DLHANDLE(CT,Run);
    
    OPENDL_API extern const dl_CFStringRef dl_kCTForegroundColorAttributeName;
    OPENDL_API extern const dl_CFStringRef dl_kCTForegroundColorFromContextAttributeName;
	OPENDL_API extern const dl_CFStringRef dl_kCTFontAttributeName;
	OPENDL_API extern const dl_CFStringRef dl_kCTStrokeWidthAttributeName;
	OPENDL_API extern const dl_CFStringRef dl_kCTStrokeColorAttributeName;
    
    OPENDL_API void dl_CGContextSetTextMatrix(dl_CGContextRef c, dl_CGAffineTransform t);
    
	OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateWithName(dl_CFStringRef name, dl_CGFloat size, const dl_CGAffineTransform *matrix);
	OPENDL_API dl_CFArrayRef CDECL dl_CTFontManagerCreateFontDescriptorsFromURL(dl_CFURLRef fileURL); // array of dl_CTFontDescriptorRef 
	OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateWithFontDescriptor(dl_CTFontDescriptorRef descriptor, dl_CGFloat size, const dl_CGAffineTransform *matrix);
    
    enum {
        dl_kCTFontClassMaskShift = 28
    };
	enum {
        dl_kCTFontTraitItalic              = (1 << 0),     // Additional detail available via kCTFontSlantTrait
        dl_kCTFontTraitBold                = (1 << 1),     // Additional detail available via kCTFontWeightTrait
        dl_kCTFontTraitExpanded            = (1 << 5),     // Expanded and condensed traits are mutually exclusive
        dl_kCTFontTraitCondensed           = (1 << 6),     // Additional detail available via kCTFontWidthTrait
        dl_kCTFontTraitMonoSpace           = (1 << 10),    // Use fixed-pitch glyphs if available. May have multiple glyph advances (most CJK glyphs may contain two spaces)
        dl_kCTFontTraitVertical            = (1 << 11),    // Use vertical glyph variants and metrics
        dl_kCTFontTraitUIOptimized         = (1 << 12),    // Synthesize appropriate attributes for UI rendering such as control titles if necessary
        dl_kCTFontTraitColorGlyphs         = (1 << 13),    // Color glyphs ('sbix' or 'COLR') are available.
        dl_kCTFontTraitComposite           = (1 << 14),    // The font is a CFR (Composite font reference), a cascade list is expected per font.
        
        dl_kCTFontTraitClassMask           = (15U << dl_kCTFontClassMaskShift),
	};
	typedef uint32_t dl_CTFontSymbolicTraits;
    
    OPENDL_API dl_CTFontRef CDECL dl_CTFontCreateCopyWithSymbolicTraits(dl_CTFontRef font, dl_CGFloat size, const dl_CGAffineTransform *matrix, dl_CTFontSymbolicTraits symTraitValue, dl_CTFontSymbolicTraits symTraitMask);

	OPENDL_API dl_CGFloat CDECL dl_CTFontGetAscent(dl_CTFontRef font);
	OPENDL_API dl_CGFloat CDECL dl_CTFontGetDescent(dl_CTFontRef font);
	OPENDL_API dl_CGFloat CDECL dl_CTFontGetUnderlineThickness(dl_CTFontRef font);
	OPENDL_API dl_CGFloat CDECL dl_CTFontGetUnderlinePosition(dl_CTFontRef font);

    OPENDL_API dl_CTFramesetterRef CDECL dl_CTFramesetterCreateWithAttributedString(dl_CFAttributedStringRef attrString);
    
    OPENDL_API dl_CTFrameRef CDECL dl_CTFramesetterCreateFrame(dl_CTFramesetterRef framesetter,
                                                   dl_CFRange stringRange, dl_CGPathRef path, dl_CFDictionaryRef frameAttributes);
    OPENDL_API dl_CFArrayRef CDECL dl_CTFrameGetLines(dl_CTFrameRef frame);
    OPENDL_API void CDECL dl_CTFrameGetLineOrigins(dl_CTFrameRef frame, dl_CFRange range, dl_CGPoint origins[]);
    OPENDL_API dl_CFDictionaryRef CDECL dl_CTRunGetAttributes(dl_CTRunRef run);
    OPENDL_API double CDECL dl_CTRunGetTypographicBounds(dl_CTRunRef run, dl_CFRange range, dl_CGFloat* ascent, dl_CGFloat* descent, dl_CGFloat* leading);
    OPENDL_API dl_CFRange CDECL dl_CTRunGetStringRange(dl_CTRunRef run);
    enum {
        dl_kCTRunStatusNoStatus = 0,
        dl_kCTRunStatusRightToLeft = (1 << 0),
        dl_kCTRunStatusNonMonotonic = (1 << 1),
        dl_kCTRunStatusHasNonIdentityMatrix = (1 << 2)
    };
    typedef uint32_t dl_CTRunStatus;
    OPENDL_API dl_CTRunStatus CDECL dl_CTRunGetStatus(dl_CTRunRef run);
    
    OPENDL_API void CDECL dl_CTFrameDraw(dl_CTFrameRef frame, dl_CGContextRef context);
    
    enum {
        dl_kCTParagraphStyleSpecifierAlignment = 0,
        dl_kCTParagraphStyleSpecifierFirstLineHeadIndent = 1,
        dl_kCTParagraphStyleSpecifierHeadIndent = 2,
        dl_kCTParagraphStyleSpecifierTailIndent = 3,
        dl_kCTParagraphStyleSpecifierTabStops = 4,
        dl_kCTParagraphStyleSpecifierDefaultTabInterval = 5,
        dl_kCTParagraphStyleSpecifierLineBreakMode = 6,
        dl_kCTParagraphStyleSpecifierLineHeightMultiple = 7,
        dl_kCTParagraphStyleSpecifierMaximumLineHeight = 8,
        dl_kCTParagraphStyleSpecifierMinimumLineHeight = 9,
        dl_kCTParagraphStyleSpecifierLineSpacing = 10,			/* deprecated */
        dl_kCTParagraphStyleSpecifierParagraphSpacing = 11,
        dl_kCTParagraphStyleSpecifierParagraphSpacingBefore = 12,
        dl_kCTParagraphStyleSpecifierBaseWritingDirection = 13,
        dl_kCTParagraphStyleSpecifierMaximumLineSpacing = 14,
        dl_kCTParagraphStyleSpecifierMinimumLineSpacing = 15,
        dl_kCTParagraphStyleSpecifierLineSpacingAdjustment = 16,
        dl_kCTParagraphStyleSpecifierLineBoundsOptions = 17,
        dl_kCTParagraphStyleSpecifierCount
    };
    typedef uint32_t dl_CTParagraphStyleSpecifier;
    
    typedef struct
    {
        dl_CTParagraphStyleSpecifier spec;
        size_t valueSize;
        const void* value;
    } dl_CTParagraphStyleSetting;

#ifdef DL_PLATFORM_MACOS
    typedef enum : uint8_t { // must specify enum size for these to work on OSX! (because sizeof() is used when passing to paragraph styles)
#else
	typedef enum {
#endif
        dl_kCTTextAlignmentLeft = 0,
        dl_kCTTextAlignmentRight = 1,
        dl_kCTTextAlignmentCenter = 2,
        dl_kCTTextAlignmentJustified = 3,
        dl_kCTTextAlignmentNatural = 4,
    } dl_CTTextAlignment;
    
    DLHANDLE(CT,ParagraphStyle);
    OPENDL_API dl_CTParagraphStyleRef CDECL dl_CTParagraphStyleCreate(const dl_CTParagraphStyleSetting * settings,
                                                          size_t settingCount);
    
    OPENDL_API extern const dl_CFStringRef dl_kCTParagraphStyleAttributeName;
    
    OPENDL_API dl_CTLineRef CDECL dl_CTLineCreateWithAttributedString(dl_CFAttributedStringRef string);
    enum {
        dl_kCTLineBoundsExcludeTypographicLeading  = 1 << 0,
        dl_kCTLineBoundsExcludeTypographicShifts   = 1 << 1,
        dl_kCTLineBoundsUseHangingPunctuation      = 1 << 2,
        dl_kCTLineBoundsUseGlyphPathBounds         = 1 << 3,
        dl_kCTLineBoundsUseOpticalBounds           = 1 << 4
    };
    typedef unsigned int dl_CTLineBoundsOptions;
    OPENDL_API dl_CGRect CDECL dl_CTLineGetBoundsWithOptions(dl_CTLineRef line, dl_CTLineBoundsOptions options);
	OPENDL_API double CDECL dl_CTLineGetTypographicBounds(dl_CTLineRef line, dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *leading);
	OPENDL_API dl_CGFloat CDECL dl_CTLineGetOffsetForStringIndex(dl_CTLineRef line, dl_CFIndex charIndex, dl_CGFloat* secondaryOffset);
	OPENDL_API dl_CFIndex CDECL dl_CTLineGetStringIndexForPosition(dl_CTLineRef line, dl_CGPoint position);
	OPENDL_API dl_CFRange CDECL dl_CTLineGetStringRange(dl_CTLineRef line);
	OPENDL_API dl_CFArrayRef CDECL dl_CTLineGetGlyphRuns(dl_CTLineRef line);
	OPENDL_API void CDECL dl_CTLineDraw(dl_CTLineRef line, dl_CGContextRef context);

    OPENDL_API void CDECL dl_CGContextSetTextPosition(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y);
    
    typedef enum {
        dl_kCGTextFill,
        dl_kCGTextStroke,
        dl_kCGTextFillStroke,
        dl_kCGTextInvisible,
        dl_kCGTextFillClip,
        dl_kCGTextStrokeClip,
        dl_kCGTextFillStrokeClip,
        dl_kCGTextClip
    } dl_CGTextDrawingMode;
    OPENDL_API void CDECL dl_CGContextSetTextDrawingMode(dl_CGContextRef c, dl_CGTextDrawingMode mode);
    
    // bitmaps ===============
    enum {
        dl_kCGImageAlphaNone,               /* For example, RGB. */
        dl_kCGImageAlphaPremultipliedLast,  /* For example, premultiplied RGBA */
        dl_kCGImageAlphaPremultipliedFirst, /* For example, premultiplied ARGB */
        dl_kCGImageAlphaLast,               /* For example, non-premultiplied RGBA */
        dl_kCGImageAlphaFirst,              /* For example, non-premultiplied ARGB */
        dl_kCGImageAlphaNoneSkipLast,       /* For example, RBGX. */
        dl_kCGImageAlphaNoneSkipFirst,      /* For example, XRGB. */
        dl_kCGImageAlphaOnly                /* No color data, alpha data only */
    };
    typedef uint32_t dl_CGImageAlphaInfo;
        
    enum {
        dl_kCGImageByteOrderMask     = 0x7000,
        dl_kCGImageByteOrder16Little = (1 << 12),
        dl_kCGImageByteOrder32Little = (2 << 12),
        dl_kCGImageByteOrder16Big    = (3 << 12),
        dl_kCGImageByteOrder32Big    = (4 << 12)
    };
    typedef uint32_t dl_CGImageByteOrderInfo;
        
    enum {
        dl_kCGBitmapAlphaInfoMask = 0x1F,
        
        dl_kCGBitmapFloatInfoMask = 0xF00,
        dl_kCGBitmapFloatComponents = (1 << 8),
        
        dl_kCGBitmapByteOrderMask     = dl_kCGImageByteOrderMask,
        dl_kCGBitmapByteOrderDefault  = (0 << 12),
        dl_kCGBitmapByteOrder16Little = dl_kCGImageByteOrder16Little,
        dl_kCGBitmapByteOrder32Little = dl_kCGImageByteOrder32Little,
        dl_kCGBitmapByteOrder16Big    = dl_kCGImageByteOrder16Big,
        dl_kCGBitmapByteOrder32Big    = dl_kCGImageByteOrder32Big
    };
    typedef uint32_t dl_CGBitmapInfo;
        
    OPENDL_API dl_CGContextRef CDECL dl_CGBitmapContextCreate(void *data, size_t width,
                                                              size_t height, size_t bitsPerComponent, size_t bytesPerRow,
                                                              dl_CGColorSpaceRef space, dl_CGBitmapInfo bitmapInfo);

	// this API differs from stock Quartz, because Windows needs to lock/unlock the data for writing
	// dl_CGBitmapContextReleaseData will be a no-op on Mac and any other platform that doesn't need locking/unlocking
	// maybe in the future have a separate CGBitmapLock object with its own GetData() method?
    OPENDL_API void * CDECL dl_CGBitmapContextGetData(dl_CGContextRef bitmap);
	OPENDL_API void CDECL dl_CGBitmapContextReleaseData(dl_CGContextRef bitmap); // non-standard: added because WICBitmap needs to lock/release data for writing
        
    DLHANDLE(CG,Image);
    OPENDL_API dl_CGImageRef CDECL dl_CGBitmapContextCreateImage(dl_CGContextRef context);
    OPENDL_API void CDECL dl_CGImageRelease(dl_CGImageRef image);
    OPENDL_API void CDECL dl_CGContextClipToMask(dl_CGContextRef c, dl_CGRect rect, dl_CGImageRef mask);
	OPENDL_API void CDECL dl_CGContextDrawImage(dl_CGContextRef c, dl_CGRect rect, dl_CGImageRef image);

    // testing ===============
	OPENDL_API void CDECL dl_InternalTesting(dl_CGContextRef c, int width, int height);

    // globals fetching for languages where direct access is a pain (eg C#) ==============
    typedef struct {
        dl_CFIndex dl_kCFNotFound;
        dl_CFRange dl_CFRangeZero;
        dl_CFBooleanRef dl_kCFBooleanTrue;
        dl_CFBooleanRef dl_kCFBooleanFalse;
        dl_CGPoint dl_CGPointZero;
        dl_CGRect dl_CGRectZero;
        dl_CGAffineTransform dl_CGAffineTransformIdentity;
        dl_CFStringRef dl_kCGColorSpaceGenericGray;
        dl_CFStringRef dl_kCGColorSpaceGenericRGB;
        dl_CFStringRef dl_kCGColorSpaceGenericCMYK;
        dl_CFStringRef dl_kCGColorSpaceGenericRGBLinear;
        dl_CFStringRef dl_kCGColorSpaceAdobeRGB1998;
        dl_CFStringRef dl_kCGColorSpaceSRGB;
        dl_CFStringRef dl_kCGColorSpaceGenericGrayGamma2_2;
        dl_CFStringRef dl_kCGColorWhite;
        dl_CFStringRef dl_kCGColorBlack;
        dl_CFStringRef dl_kCGColorClear;
        dl_CFStringRef dl_kCTForegroundColorAttributeName;
        dl_CFStringRef dl_kCTForegroundColorFromContextAttributeName;
        dl_CFStringRef dl_kCTFontAttributeName;
        dl_CFStringRef dl_kCTStrokeWidthAttributeName;
        dl_CFStringRef dl_kCTStrokeColorAttributeName;
        dl_CFStringRef dl_kCTParagraphStyleAttributeName;
    } dl_Globals;

    OPENDL_API void CDECL dl_GetGlobals(dl_Globals* dest);


#ifdef __cplusplus
}
#endif

#endif // OPENDL_LIBRARY_H
