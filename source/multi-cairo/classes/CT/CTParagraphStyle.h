#ifndef APIDEMO_CTPARAGRAPHSTYLE_H
#define APIDEMO_CTPARAGRAPHSTYLE_H

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

#include <vector>

//struct CTParagraphStyleSetting {
//    dl_CTParagraphStyleSpecifier spec;
//    union {
//        dl_CTTextAlignment alignment; // dl_kCTParagraphStyleSpecifierAlignment
//    };
//};

class CTParagraphStyle; typedef CTParagraphStyle* CTParagraphStyleRef;
class CTParagraphStyle : public cf::Object {
    std::vector<dl_CTParagraphStyleSetting> settings;
    void *dataBlock; // all the values copied, since they are provided to us as arbitrary data blocks (ptr + size)
                     // could probably do it another way but this is more fun :D
public:
    const char *getTypeName() const override;
    CTParagraphStyleRef copy() override;

    CTParagraphStyle(const dl_CTParagraphStyleSetting *settings, size_t settingCount);
    static CTParagraphStyleRef create(const dl_CTParagraphStyleSetting * settings, size_t settingCount) {
        return new CTParagraphStyle(settings, settingCount);
    }

    ~CTParagraphStyle() override;

    std::vector<dl_CTParagraphStyleSetting> getSettings() { return settings; }

    RETAIN_AND_AUTORELEASE(CTParagraphStyle);
    WEAKREF_MAKE(CTParagraphStyle);
};

#endif //APIDEMO_CTPARAGRAPHSTYLE_H
