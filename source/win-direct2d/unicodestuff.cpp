#include "unicodestuff.h"

#include <unicode/unistr.h>
//#include <boost/locale/encoding_utf.hpp>
////#include <boost/locale.hpp>
//using boost::locale::conv::utf_to_utf;

//#include <sstream>
//#include <fstream>
//#include <codecvt>

#include <Windows.h>
#include <stringapiset.h>

//std::wstring utf8_to_wstring(const std::string &utf8) {
//	auto bufSize = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
//	auto outBuffer = new wchar_t[bufSize];
//	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, outBuffer, bufSize);
//	std::wstring ret(outBuffer);
//	delete[] outBuffer;
//	return ret;
//	//return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(utf8.data());
//	//return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
//}
//
//std::string wstring_to_utf8(const std::wstring &wideStr) {
//	auto bufSize = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, NULL, 0, NULL, NULL);
//	auto outBuffer = new char[bufSize];
//	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, outBuffer, bufSize, NULL, NULL);
//	std::string ret(outBuffer);
//	delete[] outBuffer;
//	return ret;
//	//return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(wideStr);
//	//return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
//}

//std::u32string wstring_to_utf32(const std::wstring &wideStr) {
//	std::wstring_convert<std::codecvt_utf16<__int32, 0x10ffff, std::little_endian>, __int32> conv; // instead of char32_t, visual studio bug according to: https://github.com/antlr/antlr4/commit/de6f04be0beebd17ea6232f554635c5262127aaa
//	auto pData = wideStr.c_str();
//	std::u32string utf32 =
//		reinterpret_cast<const char32_t *>(
//			conv.from_bytes(
//				reinterpret_cast<const char *>(pData),
//				reinterpret_cast<const char *>(pData + wideStr.length())).data());
//	return utf32;
//}

// see here what is required for this to work: https://stackoverflow.com/questions/26990412/c-boost-crashes-while-using-locale
//std::string upperCased(std::string input) {
//    return boost::locale::to_upper(input);
//}

std::wstring utf16_to_wstring(const std::u16string & str)
{
	std::wstring ret(str.begin(), str.end());
	return ret;
}

std::string upperCased(std::string input) {
	std::string output;
	auto uni = icu::UnicodeString::fromUTF8(input);
	auto upped = uni.toUpper();
	upped.toUTF8String(output);
	return output;
}

