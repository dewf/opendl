#ifndef OPENWL_UNICODESTUFF_H
#define OPENWL_UNICODESTUFF_H

#include <string>

std::wstring utf8_to_wstring(const std::string &str);
std::string wstring_to_utf8(const std::wstring &str);
//std::u32string wstring_to_utf32(const std::wstring &wideStr);

std::string upperCased(std::string input);

#endif //OPENWL_UNICODESTUFF_H
