#include "../dlcf.h"
//#include "../../deps/CFMinimal/source/cfminimal.h"
#include "../../deps/CFMinimal/source/CF/CFTypes.h"

#define STRUCT_CAST(x, y) (*((y*)(&x)))

using namespace cf;

OPENDL_API void CDECL dl_CFRetain(dl_CFTypeRef ref)
{
	((ObjectRef)ref)->retain();
}

OPENDL_API void CDECL dl_CFRelease(dl_CFTypeRef ref)
{
	((ObjectRef)ref)->release();
}

OPENDL_API void CDECL dl_CFDump(dl_CFTypeRef ref)
{
	((ObjectRef)ref)->dump();
}

OPENDL_API void CDECL __dl_CFBeginLeakMonitoring()
{
	Object::startLeakMonitoring();
}

OPENDL_API void CDECL __dl_CFEndLeakMonitoring()
{
	Object::endLeakMonitoring();
}

OPENDL_API const dl_CFIndex dl_kCFNotFound = kCFNotFound;
OPENDL_API const dl_CFRange dl_CFRangeZero = dl_CFRangeMake(0, 0);
OPENDL_API const dl_CFBooleanRef dl_kCFBooleanTrue = (dl_CFBooleanRef)Boolean::trueValue();
OPENDL_API const dl_CFBooleanRef dl_kCFBooleanFalse = (dl_CFBooleanRef)Boolean::falseValue();

OPENDL_API bool CDECL dl_CFBooleanGetValue(dl_CFBooleanRef boolean)
{
	return ((BooleanRef)boolean)->getValue();
}

OPENDL_API dl_CFStringRef CDECL __dl_CFStringMakeConstantString(const char *cStr)
{
	return (dl_CFStringRef)String::makeConstantString(cStr);
}

OPENDL_API dl_CFStringRef CDECL dl_CFStringCreateWithCString(const char *cStr)
{
	return (dl_CFStringRef)String::createWithCString(cStr);
}

OPENDL_API dl_CFRange dl_CFStringFind(dl_CFStringRef theString, dl_CFStringRef stringToFind, dl_CFStringCompareFlags compareOptions)
{
	auto x = ((StringRef)theString)->find((StringRef)stringToFind, (CFStringCompareFlags)compareOptions);
	return STRUCT_CAST(x, dl_CFRange);
}

OPENDL_API dl_CFArrayRef dl_CFArrayCreate(dl_CFTypeRef *values, dl_CFIndex numValues)
{
	return (dl_CFArrayRef)Array::create((ObjectRef *)values, (CFIndex)numValues);
}
OPENDL_API dl_CFIndex CDECL dl_CFArrayGetCount(dl_CFArrayRef theArray)
{
	return (dl_CFIndex)((ArrayRef)theArray)->getCount();
}
OPENDL_API const void * CDECL dl_CFArrayGetValueAtIndex(dl_CFArrayRef theArray, dl_CFIndex idx)
{
	return (const void *)((ArrayRef)theArray)->getValueAtIndex((CFIndex)idx);
}

OPENDL_API dl_CFDictionaryRef CDECL dl_CFDictionaryCreate(const void **keys, const void **values, dl_CFIndex numValues)
{
	return (dl_CFDictionaryRef)Dictionary::create((ObjectRef *)keys, (ObjectRef *)values, (CFIndex)numValues);
}
OPENDL_API const void * CDECL dl_CFDictionaryGetValue(dl_CFDictionaryRef theDict, const void *key)
{
	return (void *)((DictionaryRef)theDict)->getValue((ObjectRef)key);
}
OPENDL_API dl_CFMutableDictionaryRef CDECL dl_CFDictionaryCreateMutable(dl_CFIndex capacity)
{
	return (dl_CFMutableDictionaryRef)MutableDictionary::create(); // capacity ignored for now
}
OPENDL_API void CDECL dl_CFDictionarySetValue(dl_CFMutableDictionaryRef theDict, const void *key, const void *value)
{
	((MutableDictionaryRef)theDict)->setValue((ObjectRef)key, (ObjectRef)value);
}

OPENDL_API dl_CFAttributedStringRef CDECL dl_CFAttributedStringCreate(dl_CFStringRef str, dl_CFDictionaryRef attributes)
{
	return (dl_CFAttributedStringRef)AttributedString::create((StringRef)str, (DictionaryRef)attributes);
}
OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutable(dl_CFIndex maxLength)
{
	return (dl_CFMutableAttributedStringRef) MutableAttributedString::create(); // maxlength ignored for now
}
OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutableCopy(dl_CFIndex maxLength, dl_CFAttributedStringRef aStr)
{
	return (dl_CFMutableAttributedStringRef)MutableAttributedString::create((AttributedStringRef)aStr); // maxLength ignored for now
}

OPENDL_API void CDECL dl_CFAttributedStringReplaceString(dl_CFMutableAttributedStringRef aStr, dl_CFRange range, dl_CFStringRef replacement)
{
	((MutableAttributedStringRef)aStr)->replaceString(STRUCT_CAST(range, CFRange), (StringRef)replacement);
}
OPENDL_API dl_CFIndex CDECL dl_CFAttributedStringGetLength(dl_CFAttributedStringRef aStr)
{
	return (CFIndex)((AttributedStringRef)aStr)->getLength();
}

OPENDL_API void CDECL dl_CFAttributedStringSetAttribute(dl_CFMutableAttributedStringRef aStr,
	dl_CFRange range, dl_CFStringRef attrName, dl_CFTypeRef value)
{
	((MutableAttributedStringRef)aStr)->setAttribute(STRUCT_CAST(range, CFRange), (StringRef)attrName, (ObjectRef)value);
}
OPENDL_API void CDECL dl_CFAttributedStringBeginEditing(dl_CFMutableAttributedStringRef aStr)
{
	((MutableAttributedStringRef)aStr)->beginEditing();
}
OPENDL_API void CDECL dl_CFAttributedStringEndEditing(dl_CFMutableAttributedStringRef aStr)
{
	((MutableAttributedStringRef)aStr)->endEditing();
}

OPENDL_API dl_CFNumberRef CDECL dl_CFNumberCreate(dl_CFNumberType theType, const void *valuePtr)
{
	return (dl_CFNumberRef)Number::create((CFNumberType)theType, valuePtr);
}
OPENDL_API bool CDECL dl_CFNumberGetValue(dl_CFNumberRef number, dl_CFNumberType theType, void *valuePtr)
{
	return ((NumberRef)number)->getValue((CFNumberType)theType, valuePtr);
}
OPENDL_API dl_CFNumberRef CDECL dl_CFNumberWithFloat(float value)
{
	return (dl_CFNumberRef)Number::numberWithFloat(value);
}

OPENDL_API dl_CFURLRef CDECL dl_CFURLCreateWithFileSystemPath(dl_CFStringRef filePath, dl_CFURLPathStyle pathStyle, bool isDirectory)
{
	return (dl_CFURLRef)URL::createWithFileSystemPath((StringRef)filePath, (CFURLPathStyle)pathStyle, isDirectory);
}
