#include "CFArray.h"

using namespace cf;

OPENDL_API dl_CFArrayRef dl_CFArrayCreate(dl_CFTypeRef *values, dl_CFIndex numValues)
{
	return (dl_CFArrayRef)Array::create((ObjectRef *)values, numValues);
}

OPENDL_API dl_CFIndex CDECL dl_CFArrayGetCount(dl_CFArrayRef theArray)
{
	return ((ArrayRef)theArray)->getCount();
}

OPENDL_API const void * CDECL dl_CFArrayGetValueAtIndex(dl_CFArrayRef theArray, dl_CFIndex idx)
{
	return (const void *)((ArrayRef)theArray)->getValueAtIndex(idx);
}


