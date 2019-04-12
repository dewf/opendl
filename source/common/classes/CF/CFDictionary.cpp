#include "CFDictionary.h"

#include "CFMutableDictionary.h"

using namespace cf;

DictionaryRef Dictionary::EmptyDict = makeEternal(new Dictionary());

OPENDL_API dl_CFDictionaryRef CDECL dl_CFDictionaryCreate(const void **keys, const void **values, dl_CFIndex numValues)
{
	return (dl_CFDictionaryRef) new Dictionary((ObjectRef *)keys, (ObjectRef *)values, numValues);
}

OPENDL_API const void * CDECL dl_CFDictionaryGetValue(dl_CFDictionaryRef theDict, const void *key)
{
	return (const void *)((DictionaryRef)theDict)->getValue((ObjectRef)key);
}

