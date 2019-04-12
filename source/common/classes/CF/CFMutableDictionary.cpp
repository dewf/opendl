#include "CFMutableDictionary.h"

using namespace cf;

OPENDL_API dl_CFMutableDictionaryRef CDECL dl_CFDictionaryCreateMutable(dl_CFIndex capacity)
{
	return (dl_CFMutableDictionaryRef) new MutableDictionary();
}

OPENDL_API void CDECL dl_CFDictionarySetValue(dl_CFMutableDictionaryRef theDict, const void *key, const void *value)
{
	((MutableDictionaryRef)theDict)->setValue((ObjectRef)key, (ObjectRef)value);
}
