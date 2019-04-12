#include "opendl.h"
#import <CoreFoundation/CoreFoundation.h>
#import <CoreText/CoreText.h>

#include "util.h"

#include "../common/ExoClass.h"

// boolean ========================================
OPENDL_API const dl_CFBooleanRef dl_kCFBooleanTrue = (dl_CFBooleanRef)kCFBooleanTrue;
OPENDL_API const dl_CFBooleanRef dl_kCFBooleanFalse = (dl_CFBooleanRef)kCFBooleanFalse;

OPENDL_API bool CDECL dl_CFBooleanGetValue(dl_CFBooleanRef boolean)
{
    return CFBooleanGetValue((CFBooleanRef)boolean);
}

// cfstring =======================================

#include <string>
#include <map>
static std::map<std::string, CFStringRef> constantStrings;

OPENDL_API dl_CFStringRef CDECL __dl_CFStringMakeConstantString(const char *cStr)
{
    // can't use CFSTR because it's a special compiler thing, and __CFStringMakeConstantString hates UTF-8 for some reason, so ...
    // emulate the behavior by keeping a set of strings that have been seen before
    std::string key = cStr;
    auto found = constantStrings.find(key);
    if (found != constantStrings.end()) {
        return (dl_CFStringRef) found->second;
    } else {
        // else new
        auto cfstr = CFStringCreateWithCString(kCFAllocatorDefault, cStr, kCFStringEncodingUTF8);
        constantStrings[key] = cfstr;
        return (dl_CFStringRef) cfstr;
    }
}

OPENDL_API dl_CFStringRef CDECL dl_CFStringCreateWithCString(const char *cStr)
{
    return (dl_CFStringRef) CFStringCreateWithCString(kCFAllocatorDefault, cStr, kCFStringEncodingUTF8);
}

// =================================================

OPENDL_API void CDECL dl_CFRelease(dl_CFTypeRef ref)
{
    // see if there's any ExoClass associated with this CF object ...
    auto exo = ExoClass::getFor(ref);
    if (exo) {
        delete exo;
    }
    CFRelease((CFTypeRef)ref);
}

OPENDL_API dl_CFURLRef CDECL dl_CFURLCreateWithFileSystemPath(dl_CFStringRef filePath, dl_CFURLPathStyle pathStyle, bool isDirectory)
{
    return (dl_CFURLRef)CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                   (CFStringRef)filePath,
                                                   (CFURLPathStyle)pathStyle,
                                                   isDirectory ? YES : NO);
}

// array ============================================

OPENDL_API dl_CFIndex CDECL dl_CFArrayGetCount(dl_CFArrayRef theArray)
{
    return (dl_CFIndex)CFArrayGetCount((CFArrayRef)theArray);
}

OPENDL_API const void * CDECL dl_CFArrayGetValueAtIndex(dl_CFArrayRef theArray, dl_CFIndex idx)
{
    return CFArrayGetValueAtIndex((CFArrayRef)theArray, (CFIndex)idx);
}

// dictionary =======================================

OPENDL_API dl_CFDictionaryRef CDECL dl_CFDictionaryCreate(const void **keys, const void **values, dl_CFIndex numValues)
{
    return (dl_CFDictionaryRef) CFDictionaryCreate(kCFAllocatorDefault, keys, values, (CFIndex)numValues,
                                               &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
}

OPENDL_API const void * CDECL dl_CFDictionaryGetValue(dl_CFDictionaryRef theDict, const void *key)
{
    return CFDictionaryGetValue((CFDictionaryRef)theDict, key);
}

OPENDL_API dl_CFMutableDictionaryRef CDECL dl_CFDictionaryCreateMutable(dl_CFIndex capacity)
{
    return (dl_CFMutableDictionaryRef) CFDictionaryCreateMutable(kCFAllocatorDefault, (CFIndex)capacity,
                              &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
}

OPENDL_API void CDECL dl_CFDictionarySetValue(dl_CFMutableDictionaryRef theDict, const void *key, const void *value)
{
    CFDictionarySetValue((CFMutableDictionaryRef)theDict, key, value);
}

// number ===========================================

OPENDL_API dl_CFNumberRef dl_CFNumberCreate(dl_CFNumberType theType, const void *valuePtr)
{
    return (dl_CFNumberRef)CFNumberCreate(kCFAllocatorDefault, (CFNumberType)theType, valuePtr);
}

OPENDL_API bool CDECL dl_CFNumberGetValue(dl_CFNumberRef number, dl_CFNumberType theType, void *valuePtr)
{
    return CFNumberGetValue((CFNumberRef)number, (CFNumberType)theType, valuePtr);
}

OPENDL_API dl_CFNumberRef dl_CFNumberWithFloat(float value)
{
    return (dl_CFNumberRef)CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat32Type, &value);
}

OPENDL_API dl_CFAttributedStringRef CDECL dl_CFAttributedStringCreate(dl_CFStringRef str, dl_CFDictionaryRef attributes)
{
    return (dl_CFAttributedStringRef) CFAttributedStringCreate(kCFAllocatorDefault, (CFStringRef)str, (CFDictionaryRef)attributes);
}

OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutable(dl_CFIndex maxLength)
{
    return (dl_CFMutableAttributedStringRef) CFAttributedStringCreateMutable(kCFAllocatorDefault, (CFIndex)maxLength);
}

OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutableCopy(dl_CFIndex maxLength, dl_CFAttributedStringRef aStr)
{
    return (dl_CFMutableAttributedStringRef) CFAttributedStringCreateMutableCopy(kCFAllocatorDefault, (CFIndex)maxLength, (CFAttributedStringRef)aStr);
}

OPENDL_API void CDECL dl_CFAttributedStringReplaceString(dl_CFMutableAttributedStringRef aStr, dl_CFRange range, dl_CFStringRef replacement)
{
    CFAttributedStringReplaceString((CFMutableAttributedStringRef)aStr, UGLYCAST(CFRange, range), (CFStringRef)replacement);
}

OPENDL_API dl_CFIndex CDECL dl_CFAttributedStringGetLength(dl_CFAttributedStringRef aStr)
{
    return CFAttributedStringGetLength((CFAttributedStringRef)aStr);
}

OPENDL_API void CDECL dl_CFAttributedStringSetAttribute(dl_CFMutableAttributedStringRef aStr,
                                                     dl_CFRange range, dl_CFStringRef attrName, dl_CFTypeRef value)
{
    CFAttributedStringSetAttribute((CFMutableAttributedStringRef)aStr, UGLYCAST(CFRange, range), (CFStringRef)attrName, (CFTypeRef)value);
}

OPENDL_API void CDECL dl_CFAttributedStringBeginEditing(dl_CFMutableAttributedStringRef aStr)
{
    CFAttributedStringBeginEditing((CFMutableAttributedStringRef)aStr);
}

OPENDL_API void CDECL dl_CFAttributedStringEndEditing(dl_CFMutableAttributedStringRef aStr)
{
    CFAttributedStringEndEditing((CFMutableAttributedStringRef)aStr);
}

OPENDL_API dl_CFRange dl_CFStringFind(dl_CFStringRef theString, dl_CFStringRef stringToFind, dl_CFStringCompareFlags compareOptions)
{
    auto ret = CFStringFind((CFStringRef)theString, (CFStringRef)stringToFind, (CFStringCompareFlags)compareOptions);
    return UGLYCAST(dl_CFRange, ret);
}

