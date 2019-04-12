#pragma once

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"
#include "../../private_defs.h"
#include "../../unicodestuff.h"

#include <vector>

//#include "../CT/CTFont.h"
//#include "../../directwrite/MyTextRenderer.h"
//#include "../../directwrite/CoreTextFormatSpec.h"
//#include "../../directwrite/CoreTextRenderer.h"

class CTFontDescriptor; typedef CTFontDescriptor* CTFontDescriptorRef;
class CTFontDescriptor : public cf::Object {
	IDWriteFontFile *ff = nullptr;
	DWRITE_FONT_FILE_TYPE fileType;
	DWRITE_FONT_FACE_TYPE faceType;
	UINT32 faceIndex;
public:
	const char *getTypeName() const override {
		return "FontDescriptor";
	}

	CTFontDescriptor(IDWriteFontFile *ff,
		DWRITE_FONT_FILE_TYPE fileType,
		DWRITE_FONT_FACE_TYPE faceType,
		UINT32 faceIndex)
		:ff(ff), fileType(fileType), faceType(faceType), faceIndex(faceIndex)
	{
		ff->AddRef(); // each of these descriptors will have its own reference
	}

	~CTFontDescriptor() {
		SafeRelease(&ff);
	}

	IDWriteFontFile *getFontFile() {
		return ff;
	}

	IDWriteFontFace *createFace() {
		IDWriteFontFace *ret = nullptr;
		auto hr = writeFactory->CreateFontFace(faceType, 1, &ff, 0, DWRITE_FONT_SIMULATIONS_NONE, &ret);
		if (SUCCEEDED(hr)) {
			return ret;
		}
		else {
			printf("FontDescriptor.getFace() failed to create font face\n");
			return nullptr;
		}
	}

	CTFontDescriptorRef copy() override {
		// immutable
		return (CTFontDescriptorRef)retain();
	}

	static cf::ArrayRef createDescriptorsFromURL(cf::URLRef url) {
		auto path = url->copyFileSystemPath(dl_kCFURLWindowsPathStyle);
		auto str = path->getStdString();
		auto wideStr = utf8_to_wstring(str);

		IDWriteFontFile *ff;
		auto hr = writeFactory->CreateFontFileReference(wideStr.c_str(), NULL, &ff);
		if (FAILED(hr)) {
			printf("failed to load font file: %s\n", str.c_str());
			return nullptr;
		}

		cf::ArrayRef ret = nullptr;

		BOOL isSupported;
		DWRITE_FONT_FILE_TYPE fileType;
		DWRITE_FONT_FACE_TYPE faceType;
		UINT32 numFaces;
		HR(ff->Analyze(&isSupported, &fileType, &faceType, &numFaces));

		if (isSupported && numFaces > 0) {
			std::vector<CTFontDescriptorRef> items;
			// create new desc for every face ... I guess
			for (UINT32 i = 0; i < numFaces; i++) {
				auto fd = new CTFontDescriptor(ff, fileType, faceType, i);
				items.push_back(fd);
				printf(" - created descriptor for font [%s] #%d\n", str.c_str(), i);
			}
			ret = cf::Array::create((cf::ObjectRef *)items.data(), numFaces);
		}

		ff->Release(); // each of the descriptors (if any) got its own reference (via AddRef()), so we can free this one
		path->release();

		return ret;
	}
};

