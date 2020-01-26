#include "unicodestuff.h"

#include <Windows.h>
#include <stringapiset.h>

std::wstring utf16_to_wstring(const std::u16string & str)
{
	std::wstring ret(str.begin(), str.end());
	return ret;
}
