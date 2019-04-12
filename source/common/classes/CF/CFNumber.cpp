#include "CFNumber.h"

using namespace cf;

OPENDL_API dl_CFNumberRef CDECL dl_CFNumberCreate(dl_CFNumberType theType, const void *valuePtr)
{
	return (dl_CFNumberRef) new Number(theType, valuePtr);
}

OPENDL_API bool CDECL dl_CFNumberGetValue(dl_CFNumberRef number, dl_CFNumberType theType, void *valuePtr)
{
	return ((NumberRef)number)->getValue(theType, valuePtr);
}

OPENDL_API dl_CFNumberRef CDECL dl_CFNumberWithFloat(float value)
{
	return (dl_CFNumberRef)Number::numberWithFloat(value);
}
