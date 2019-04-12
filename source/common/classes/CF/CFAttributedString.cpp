#include "CFAttributedString.h"

using namespace cf;

OPENDL_API dl_CFAttributedStringRef CDECL dl_CFAttributedStringCreate(dl_CFStringRef str, dl_CFDictionaryRef attributes)
{
	return (dl_CFAttributedStringRef) new AttributedString((StringRef)str, (DictionaryRef)attributes);
}

OPENDL_API dl_CFIndex CDECL dl_CFAttributedStringGetLength(dl_CFAttributedStringRef aStr)
{
	return ((AttributedStringRef)aStr)->getLength();
}
