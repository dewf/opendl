#include "DWriteFontUtils.h"

#include "../private_defs.h"
#include "../unicodestuff.h"

// font map (postscript name mapping)
wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
#define LOCALE_SPECIFIC_BUFFER_SIZE 4096
wchar_t localeSpecificBuffer[LOCALE_SPECIFIC_BUFFER_SIZE];

std::wstring getLocaleSpecificName(IDWriteLocalizedStrings *strings) {
	static auto hasDefLocale = (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) > 0);

	UINT32 index = -1;
	BOOL exists = FALSE;
	if (hasDefLocale) {
		HR(strings->FindLocaleName(localeName, &index, &exists));
	}
	if (!exists) {
		strings->FindLocaleName(L"en-us", &index, &exists);
	}
	if (!exists) {
		index = 0; // just choose first locale
	}

	//UINT32 nameLength;
	//familyNames->GetStringLength(index, &nameLength);
	//assert(nameLength < FONT_NAME_BUFFER_SIZE);
	HR(strings->GetString(index, localeSpecificBuffer, LOCALE_SPECIFIC_BUFFER_SIZE));
	return std::wstring(localeSpecificBuffer);
}

void createFontMap() {
	IDWriteFontCollection *fontCollection;
	HR(writeFactory->GetSystemFontCollection(&fontCollection));

	auto familyCount = fontCollection->GetFontFamilyCount();
	for (UINT32 i = 0; i < familyCount; i++) {
		IDWriteFontFamily *family;
		HR(fontCollection->GetFontFamily(i, &family));

		IDWriteLocalizedStrings *familyNameStrings;
		HR(family->GetFamilyNames(&familyNameStrings));

		auto familyName = getLocaleSpecificName(familyNameStrings);
		auto family_utf8 = wstring_to_utf8(familyName);

		// get all fonts in family + postscript names
		UINT32 fontCount = family->GetFontCount();
		for (UINT32 j = 0; j < fontCount; j++) {
			IDWriteFont *font;
			HR(family->GetFont(j, &font));

			IDWriteLocalizedStrings *fullNameStrings;
			BOOL fullNameStringsExist = FALSE;
			HR(font->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_FULL_NAME, &fullNameStrings, &fullNameStringsExist));
			auto fullName = fullNameStringsExist ? getLocaleSpecificName(fullNameStrings) : std::wstring(L"(unknown)");

			IDWriteLocalizedStrings *postscriptNameStrings;
			BOOL postscriptNameStringsExist = FALSE;
			HR(font->GetInformationalStrings(DWRITE_INFORMATIONAL_STRING_POSTSCRIPT_NAME, &postscriptNameStrings, &postscriptNameStringsExist));
			auto postscriptName = postscriptNameStringsExist ? getLocaleSpecificName(postscriptNameStrings) : std::wstring(L"(unknown)");

			if (postscriptNameStringsExist) {
				auto ps_utf8 = wstring_to_utf8(postscriptName);
				//printf("%s -> %LS [%d]\n", ps_utf8.c_str(), fullName.c_str(), font->GetStretch());
				// map ps name to family name | weight | style | stretch
			}

			postscriptNameStrings->Release();
			fullNameStrings->Release();
			font->Release();
		}

		familyNameStrings->Release();
		family->Release();
	}
	fontCollection->Release();
}
