#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include "../../COMStuff.h"
#include <dwrite.h>
#include "../../unicodestuff.h"
#include "../../private_defs.h"

#include "CTFontDescriptor.h"

#include <dwrite_3.h>

class CTFont; typedef CTFont* CTFontRef;
class CTFont : public cf::Object {
private:
	IDWriteTextFormat *format = nullptr; // ... until we find out that this sets too many traits (weight/style/stretch/etc)
	std::wstring wideName;
	//DWRITE_FONT_STYLE style;
	//FLOAT size = -1;
	IDWriteFontFace *fontFace = nullptr;
	DWRITE_FONT_METRICS metrics = {};
	IDWriteFontCollection1 *collection1 = nullptr;
public:
	const char *getTypeName() const override { return "CTFont"; }

	CTFont(CTFontDescriptorRef desc, dl_CGFloat size, const dl_CGAffineTransform *matrix)
	{
		IDWriteFactory5 *wf5;
		HR(writeFactory->QueryInterface(__uuidof(IDWriteFactory5), (void **)&wf5));

		IDWriteFontSetBuilder1 *builder;
		HR(wf5->CreateFontSetBuilder(&builder));

		builder->AddFontFile(desc->getFontFile());

		IDWriteFontSet *fontSet;
		HR(builder->CreateFontSet(&fontSet));
		
		// collection is a class member
		HR(wf5->CreateFontCollectionFromFontSet(fontSet, &collection1));

		// create face
		fontFace = desc->createFace();

		IDWriteFont *font;
		HR(collection1->GetFontFromFontFace(fontFace, &font));

		IDWriteFontFamily *family;
		HR(font->GetFontFamily(&family));

		IDWriteLocalizedStrings *familyNameStrings;
		HR(family->GetFamilyNames(&familyNameStrings));

		UINT32 index;
		BOOL exists;
		HR(familyNameStrings->FindLocaleName(L"en-us", &index, &exists)); // should we use GetUserDefaultLocaleName to get this first?
		if (!exists) {
			throw cf::Exception("font family name for locale en-us doesn't exist");
		}

		UINT32 familyNameLen;
		HR(familyNameStrings->GetStringLength(index, &familyNameLen));
		auto familyNameBuffer = new wchar_t[familyNameLen + 1];
		HR(familyNameStrings->GetString(index, familyNameBuffer, familyNameLen + 1));
		wideName = familyNameBuffer;

		// fill in other fields
		HR(writeFactory->CreateTextFormat(
			familyNameBuffer,
			collection1,
			font->GetWeight(),
			font->GetStyle(),
			font->GetStretch(),
			pointsToDIP((FLOAT)size),
			L"en-us",
			&format));

		font->GetMetrics(&metrics);

		// cleanup all the residue
		delete[] familyNameBuffer;
		familyNameStrings->Release();
		family->Release();
		font->Release();
		fontSet->Release();
		builder->Release();
		wf5->Release();
	}

	static inline CTFontRef createWithFontDescriptor(CTFontDescriptorRef desc, dl_CGFloat size, const dl_CGAffineTransform *matrix)
	{
		return new CTFont(desc, size, matrix);
	}

	static CTFontRef createWithName(cf::StringRef name, dl_CGFloat size, const dl_CGAffineTransform *matrix)
	{
		auto utf16 = ((cf::StringRef)name)->getUtf16String();
		auto wideName = utf16_to_wstring(utf16);

		IDWriteTextFormat *format;
		HR(writeFactory->CreateTextFormat(
			wideName.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, //style,
			DWRITE_FONT_STRETCH_NORMAL,
			pointsToDIP((float)size),
			L"en-US",
			&format));

		//format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); // should we use this by default??
		auto result = new CTFont(format, matrix);
		format->Release();
		return result;
	}

	CTFont(IDWriteTextFormat *format, const dl_CGAffineTransform *matrix)
		:format(format)
	{
		format->AddRef();

		auto len = format->GetFontFamilyNameLength();
		auto buffer = new wchar_t[len + 1];
		HR(format->GetFontFamilyName(buffer, len + 1));
		wideName = buffer;
		delete[] buffer;

		// create the face because we need it for some stuff
		// such an asspain ...
		//HR(writeFactory->GetSystemFontCollection(&collection, FALSE));
		IDWriteFontCollection *collection;
		HR(format->GetFontCollection(&collection));

		// store collection1 for later use
		HR(collection->QueryInterface(__uuidof(IDWriteFontCollection1), (void **)&collection1));

		//WCHAR familyName[1024];
		//HR(format->GetFontFamilyName(familyName, 1024));
		UINT32 familyIndex;
		BOOL exists;
		HR(collection->FindFamilyName(wideName.c_str(), &familyIndex, &exists));
		if (!exists) {
			throw cf::Exception("failed to find index for font family name");
		}

		IDWriteFontFamily *family;
		HR(collection->GetFontFamily(familyIndex, &family));

		// get font handle for creating face then metrics
		IDWriteFont *font;
		HR(family->GetFirstMatchingFont(
			format->GetFontWeight(),
			format->GetFontStretch(),
			format->GetFontStyle(),
			&font));

		HR(font->CreateFontFace(&fontFace));
		fontFace->GetMetrics(&metrics);

		// rel stuff
		font->Release();
		family->Release();
		collection->Release();
	}

	CTFontRef copyWithSymbolicTraits(dl_CGFloat size, const dl_CGAffineTransform *matrix, dl_CTFontSymbolicTraits symTraitValue, dl_CTFontSymbolicTraits symTraitMask)
	{
		// get family / weight / style / stretch / size
		auto weight = format->GetFontWeight();
		if (symTraitMask & dl_kCTFontTraitBold) {
			weight = (symTraitValue & dl_kCTFontTraitBold) ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
		}
		auto style = format->GetFontStyle();
		if (symTraitMask & dl_kCTFontTraitItalic) {
			style = (symTraitValue & dl_kCTFontTraitItalic) ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
		}
		auto stretch = format->GetFontStretch();
		if (symTraitMask & dl_kCTFontTraitExpanded) {
			stretch = (symTraitValue & dl_kCTFontTraitExpanded) ? DWRITE_FONT_STRETCH_EXPANDED : DWRITE_FONT_STRETCH_NORMAL;
		}
		else if (symTraitMask & dl_kCTFontTraitCondensed) {
			stretch = (symTraitValue & dl_kCTFontTraitCondensed) ? DWRITE_FONT_STRETCH_CONDENSED : DWRITE_FONT_STRETCH_NORMAL;
		}
		if (size == 0) {
			size = format->GetFontSize();
		}
		else {
			size = pointsToDIP((FLOAT)size);
		}

		IDWriteTextFormat *newFormat;
		HR(writeFactory->CreateTextFormat(wideName.c_str(), collection1, weight, style, stretch, (FLOAT)size, L"en-us", &newFormat));

		auto result = new CTFont(newFormat, matrix);
		newFormat->Release(); // ctor retains

		return result;
	}

	~CTFont() {
		::SafeRelease(&format);
		::SafeRelease(&fontFace);
		::SafeRelease(&collection1);
	}

	void applyToLayout(IDWriteTextLayout *layout, DWRITE_TEXT_RANGE range)
	{
		layout->SetFontCollection(collection1, range);
		layout->SetFontFamilyName(wideName.c_str(), range);
		layout->SetFontSize(format->GetFontSize(), range);
		layout->SetFontStyle(format->GetFontStyle(), range);
		// + weight/stretch/etc
	}

	//
	IDWriteTextFormat *getTextFormat() const { return format; }
	//

	dl_CGFloat getAscent() {
		return (metrics.ascent / (dl_CGFloat)metrics.designUnitsPerEm) * pointsToDIP(format->GetFontSize());
	}

	dl_CGFloat getDescent() {
		return (metrics.descent / (dl_CGFloat)metrics.designUnitsPerEm) * pointsToDIP(format->GetFontSize());
	}

	dl_CGFloat getUnderlineThickness() {
		return (metrics.underlineThickness / (dl_CGFloat)metrics.designUnitsPerEm) * pointsToDIP(format->GetFontSize());
	}

	dl_CGFloat getUnderlinePosition() {
		return (metrics.underlinePosition / (dl_CGFloat)metrics.designUnitsPerEm) * pointsToDIP(format->GetFontSize()) * -1.0;
	}

	static CTFontRef getDefaultFont() {
		static CTFontRef ret = nullptr;
		if (!ret) {
			ret = createWithName(cf::String::makeConstantString("Arial"), 12.0, NULL);
			ret->isEternal = true;
		}
		return ret;
	}

	//void getSimpleMetrics(dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *spacing) {
	//	if (ascent) {
	//		*ascent = (metrics.ascent / (dl_CGFloat)metrics.designUnitsPerEm) * pointsToDIP(size);
	//	}
	//	if (descent) {
	//		*descent = (metrics.descent / (dl_CGFloat)metrics.designUnitsPerEm) * pointsToDIP(size);
	//	}
	//	if (spacing) {
	//		*spacing = (metrics.lineGap / (dl_CGFloat)metrics.designUnitsPerEm) * pointsToDIP(size);
	//	}
	//}

	virtual CTFontRef copy() {
		// immutable so return self
		return (CTFontRef)retain();
	}

	RETAIN_AND_AUTORELEASE(CTFont)
};
