#include "CFURL.h"

using namespace cf;

StringRef URL::backslash = String::makeConstantString("\\");
StringRef URL::fwdslash = String::makeConstantString("/");

// API methods =======================

OPENDL_API dl_CFURLRef CDECL dl_CFURLCreateWithFileSystemPath(dl_CFStringRef filePath, dl_CFURLPathStyle pathStyle, bool isDirectory)
{
	return (dl_CFURLRef)URL::createWithFileSystemPath((StringRef)filePath, pathStyle, isDirectory);
}
