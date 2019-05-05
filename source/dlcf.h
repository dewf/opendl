#ifndef __DLCF_H__
#define __DLCF_H__

#include "prologue.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // Minimal Core Foundation stuff ======
    typedef const void *dl_CFTypeRef;
    OPENDL_API void CDECL dl_CFRelease(dl_CFTypeRef ref);

    OPENDL_API void CDECL dl_CFDump(dl_CFTypeRef ref);

	// helpful to try to track down DLCF memory leaks occurring in a section of code
#ifndef DL_PLATFORM_MACOS
	OPENDL_API void CDECL __dl_CFBeginLeakMonitoring();
	OPENDL_API void CDECL __dl_CFEndLeakMonitoring();
#endif

    typedef ssize_t dl_CFIndex;
	OPENDL_API extern const dl_CFIndex dl_kCFNotFound;
    typedef struct {
        dl_CFIndex location;
        dl_CFIndex length;
    } dl_CFRange;
    OPENDL_API extern const dl_CFRange dl_CFRangeZero;

    inline dl_CFRange CDECL dl_CFRangeMake(dl_CFIndex location, dl_CFIndex length)
    {
        dl_CFRange ret;
        ret.location = location;
        ret.length = length;
        return ret;
    }
	inline dl_CFIndex dl_CFRangeEnd(dl_CFRange range) {
		return range.location + range.length;
	}
    
    DLHANDLE(CF,Boolean);
    OPENDL_API extern const dl_CFBooleanRef dl_kCFBooleanTrue;
    OPENDL_API extern const dl_CFBooleanRef dl_kCFBooleanFalse;
    OPENDL_API bool CDECL dl_CFBooleanGetValue(dl_CFBooleanRef boolean);
    
    DLHANDLE(CF,String);
	OPENDL_API dl_CFStringRef CDECL __dl_CFStringMakeConstantString(const char *cStr);
    OPENDL_API dl_CFStringRef CDECL dl_CFStringCreateWithCString(const char *cStr);
#ifdef DL_PLATFORM_MACOS
#   define dl_CFSTR(x) ((dl_CFStringRef)CFSTR(x))
#else
#	define dl_CFSTR(x) __dl_CFStringMakeConstantString("" x "")
#endif
    enum {
        dl_kCFCompareCaseInsensitive = 1,
        dl_kCFCompareBackwards = 4,		/* Starting from the end of the string */
        dl_kCFCompareAnchored = 8,		    /* Only at the specified starting point */
        dl_kCFCompareNonliteral = 16,		/* If specified, loose equivalence is performed (o-umlaut == o, umlaut) */
        dl_kCFCompareLocalized = 32,		/* User's default locale is used for the comparisons */
        dl_kCFCompareNumerically = 64,		/* Numeric comparison is used; that is, Foo2.txt < Foo7.txt < Foo25.txt */
        dl_kCFCompareDiacriticInsensitive = 128, /* If specified, ignores diacritics (o-umlaut == o) */
        dl_kCFCompareWidthInsensitive = 256, /* If specified, ignores width differences ('a' == UFF41) */
        dl_kCFCompareForcedOrdering = 512 /* If specified, comparisons are forced to return either kCFCompareLessThan or kCFCompareGreaterThan if the strings are equivalent but not strictly equal, for stability when sorting (e.g. "aaa" > "AAA" with kCFCompareCaseInsensitive specified) */
    };
    typedef unsigned int dl_CFStringCompareFlags;
    OPENDL_API dl_CFRange dl_CFStringFind(dl_CFStringRef theString, dl_CFStringRef stringToFind, dl_CFStringCompareFlags compareOptions);
    
    DLHANDLE(CF,Array);
	OPENDL_API dl_CFArrayRef dl_CFArrayCreate(dl_CFTypeRef *values, dl_CFIndex numValues);
	OPENDL_API dl_CFIndex CDECL dl_CFArrayGetCount(dl_CFArrayRef theArray);
    OPENDL_API const void * CDECL dl_CFArrayGetValueAtIndex(dl_CFArrayRef theArray, dl_CFIndex idx);
    
    DLHANDLE_WITH_MUTABLE(CF,Dictionary);
    OPENDL_API dl_CFDictionaryRef CDECL dl_CFDictionaryCreate(const void **keys, const void **values, dl_CFIndex numValues);
    OPENDL_API const void * CDECL dl_CFDictionaryGetValue(dl_CFDictionaryRef theDict, const void *key);
    OPENDL_API dl_CFMutableDictionaryRef CDECL dl_CFDictionaryCreateMutable(dl_CFIndex capacity);
    OPENDL_API void CDECL dl_CFDictionarySetValue(dl_CFMutableDictionaryRef theDict, const void *key, const void *value);
    
    DLHANDLE_WITH_MUTABLE(CF,AttributedString);
    OPENDL_API dl_CFAttributedStringRef CDECL dl_CFAttributedStringCreate(dl_CFStringRef str, dl_CFDictionaryRef attributes);
    OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutable(dl_CFIndex maxLength);
    OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutableCopy(dl_CFIndex maxLength, dl_CFAttributedStringRef aStr);

    OPENDL_API void CDECL dl_CFAttributedStringReplaceString(dl_CFMutableAttributedStringRef aStr, dl_CFRange range, dl_CFStringRef replacement);
    OPENDL_API dl_CFIndex CDECL dl_CFAttributedStringGetLength(dl_CFAttributedStringRef aStr);
    
    OPENDL_API void CDECL dl_CFAttributedStringSetAttribute(dl_CFMutableAttributedStringRef aStr,
                                                         dl_CFRange range, dl_CFStringRef attrName, dl_CFTypeRef value);

	OPENDL_API void CDECL dl_CFAttributedStringBeginEditing(dl_CFMutableAttributedStringRef aStr);
	OPENDL_API void CDECL dl_CFAttributedStringEndEditing(dl_CFMutableAttributedStringRef aStr);
    
    DLHANDLE(CF,Number);
    typedef enum {
        /* Fixed-width types */
        dl_kCFNumberSInt8Type = 1,
        dl_kCFNumberSInt16Type = 2,
        dl_kCFNumberSInt32Type = 3,
        dl_kCFNumberSInt64Type = 4,
        dl_kCFNumberFloat32Type = 5,
        dl_kCFNumberFloat64Type = 6,	/* 64-bit IEEE 754 */
        /* Basic C types */
        dl_kCFNumberCharType = 7,
        dl_kCFNumberShortType = 8,
        dl_kCFNumberIntType = 9,
        dl_kCFNumberLongType = 10,
        dl_kCFNumberLongLongType = 11,
        dl_kCFNumberFloatType = 12,
        dl_kCFNumberDoubleType = 13,
        /* Other */
        dl_kCFNumberCFIndexType = 14,
        dl_kCFNumberNSIntegerType = 15,
        dl_kCFNumberCGFloatType = 16,
        dl_kCFNumberMaxType = 16
    } dl_CFNumberType;
    OPENDL_API dl_CFNumberRef CDECL dl_CFNumberCreate(dl_CFNumberType theType, const void *valuePtr);
    OPENDL_API bool CDECL dl_CFNumberGetValue(dl_CFNumberRef number, dl_CFNumberType theType, void *valuePtr);
    //
    OPENDL_API dl_CFNumberRef CDECL dl_CFNumberWithFloat(float value); // NS-like helper routine, not part of CoreFoundation
    
    DLHANDLE(CF,URL);
    typedef enum {
        dl_kCFURLPOSIXPathStyle = 0,
        // type 1 is deprecated
        dl_kCFURLWindowsPathStyle = 2
    } dl_CFURLPathStyle;
    OPENDL_API dl_CFURLRef CDECL dl_CFURLCreateWithFileSystemPath(dl_CFStringRef filePath, dl_CFURLPathStyle pathStyle, bool isDirectory);
    
#ifdef __cplusplus
}
#endif


#endif // __DLCF_H__
