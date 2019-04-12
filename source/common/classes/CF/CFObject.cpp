#include "CFObject.h"

using namespace cf;

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
