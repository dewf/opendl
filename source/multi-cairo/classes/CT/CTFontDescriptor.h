#ifndef APIDEMO_CTFONTDESCRIPTOR_H
#define APIDEMO_CTFONTDESCRIPTOR_H

#include "../../../opendl.h"
#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"

class CTFont;

class CTFontDescriptor; typedef CTFontDescriptor* CTFontDescriptorRef;
class CTFontDescriptor : public cf::Object {
    std::string path;
    std::string familyName;
    int index; // always 0 for now
public:
    const char *getTypeName() const override;
    CTFontDescriptorRef copy() override;

    CTFontDescriptor(std::string path, int index, std::string familyName);
    static cf::ArrayRef createFontDescriptorsFromURL(cf::URLRef fileURL);

    std::string getPath() { return path; }
    int getIndex() { return index; }
    std::string getFamilyName() { return familyName; }

    RETAIN_AND_AUTORELEASE(CTFontDescriptor);
};

#endif //APIDEMO_CTFONTDESCRIPTOR_H
