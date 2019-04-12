#include <cstring>
#include "CTParagraphStyle.h"

const char *CTParagraphStyle::getTypeName() const
{
    return "CTParagraphStyle";
}

CTParagraphStyleRef CTParagraphStyle::copy()
{
    // immutable
    return retain();
}

CTParagraphStyle::CTParagraphStyle(const dl_CTParagraphStyleSetting *settings, size_t settingCount)
{
    // allocate a data block big enough to hold all the values
    size_t dataSize = 0;
    for (size_t i = 0; i < settingCount; i++) {
        dataSize += settings[i].valueSize;
    }
    dataBlock = malloc(dataSize);

    auto ptr = (uint8_t *)dataBlock;
    for (size_t i = 0; i < settingCount; i++) {
        memcpy(ptr, settings[i].value, settings[i].valueSize);
        this->settings.push_back(dl_CTParagraphStyleSetting{ settings[i].spec, settings[i].valueSize, ptr });
        ptr += settings[i].valueSize;
    };
}

CTParagraphStyle::~CTParagraphStyle() {
    free(dataBlock);
}


