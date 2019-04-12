#pragma once

#include "../../../opendl.h"
#include "../../../common/classes/CF/CFTypes.h"

#include <vector>

class CTParagraphStyle; typedef CTParagraphStyle* CTParagraphStyleRef;
class CTParagraphStyle : public cf::Object { 
public:
	// data is public for convenience
	struct CTParagraphStyleSetting {
		dl_CTParagraphStyleSpecifier spec;
		union {
			dl_CTTextAlignment alignment; // dl_kCTParagraphStyleSpecifierAlignment
		};
	};
	std::vector<CTParagraphStyleSetting> settings;

public:
	const char *getTypeName() const override { return "CTParagraphStyle"; }

	CTParagraphStyle(const dl_CTParagraphStyleSetting *settings_in, size_t settingCount)
	{
		// copy known settings
		for (size_t i = 0; i < settingCount; i++) {
			auto &setting = settings_in[i];
			CTParagraphStyleSetting ctps = { setting.spec };

			switch (setting.spec) {
			case dl_kCTParagraphStyleSpecifierAlignment:
				ctps.alignment = *((dl_CTTextAlignment *)setting.value);
				break;
			default:
				continue; // continue without adding
			}
			// otherwise we have a valid value
			settings.push_back(ctps);
		}
	}
	~CTParagraphStyle() override {}

	CTParagraphStyleRef copy() override
	{
		// immutable, retain
		return (CTParagraphStyleRef)retain();
	}

	RETAIN_AND_AUTORELEASE(CTParagraphStyle)
};

