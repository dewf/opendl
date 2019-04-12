#include "CFString.h"

#include <algorithm>
#include <cctype>

#include "CFMutableString.h"

using namespace cf;

StringRef String::makeConstantString(const char *cStr)
{
	static std::map<std::string, StringRef> constantStrings; // if this is outside the function there's no guarantee it's initialized before first use
	//
	auto key = std::string(cStr);

	auto found = constantStrings.find(key);
	if (found != constantStrings.end()) {
		return found->second;
	}
	else {
		// create anew
		auto ret = new String(cStr);
		ret->isEternal = true;
		constantStrings[key] = (StringRef)ret->retain();
		return ret;
	}
}

dl_CFRange String::find(StringRef toFind, dl_CFStringCompareFlags compareOptions)
{
	auto supportedFlags = dl_kCFCompareCaseInsensitive;
	if (compareOptions & ~supportedFlags) {
		throw Exception("String::find - unsupported compare option flags");
	}
	// else
	std::string::iterator found = str.end();
	if (compareOptions & dl_kCFCompareCaseInsensitive) {
		found = std::search(
			str.begin(), str.end(),
			toFind->str.begin(), toFind->str.end(),
			[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2);  }
		);
	}
	else {
		auto index = str.find(toFind->str, 0);
		if (index != std::string::npos) {
			return dl_CFRangeMake((dl_CFIndex)index, (dl_CFIndex)toFind->str.length());
		}
	}
	if (found != str.end()) {
		return dl_CFRangeMake((dl_CFIndex)(found - str.begin()), (dl_CFIndex)toFind->str.length());
	}
	// else
	return dl_CFRangeMake(-1, 0);
}

// defined here because of circular dependencies
MutableStringRef String::createMutableCopy() {
	return MutableString::createFromString(this);
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
	return ((StringRef)theString)->find((StringRef)stringToFind, compareOptions);
}

