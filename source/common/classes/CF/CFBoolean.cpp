#include "CFBoolean.h"

using namespace cf;

const dl_CFBooleanRef dl_kCFBooleanTrue = (dl_CFBooleanRef)Boolean::trueValue();
const dl_CFBooleanRef dl_kCFBooleanFalse = (dl_CFBooleanRef)Boolean::falseValue();

OPENDL_API bool CDECL dl_CFBooleanGetValue(dl_CFBooleanRef boolean)
{
	return ((BooleanRef)boolean)->getValue();
}
