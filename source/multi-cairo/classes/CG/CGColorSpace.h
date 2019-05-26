//
// Created by dang on 9/24/18.
//

#ifndef __CG_COLORSPACE_H__
#define __CG_COLORSPACE_H__

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

class CGColorSpace; typedef CGColorSpace* CGColorSpaceRef;
class CGColorSpace : public cf::Object {
public:
    enum ColorSpaceEnum {
        ColorSpace_None,
        ColorSpace_Named,
        ColorSpace_DeviceDependent
    };
    enum DeviceSpaceEnum {
        DeviceSpace_Gray
    };
private:
    // ==============
    ColorSpaceEnum spaceType = ColorSpace_None;
    union {
        struct {
            cf::StringRef name = nullptr;
        } named;
        struct {
            DeviceSpaceEnum which = DeviceSpace_Gray;
        } device;
    };
public:
    const char *getTypeName() const override {
        return "CGColorSpace";
    }
    CGColorSpaceRef copy() override {
        // immutable .. for now
        return (CGColorSpaceRef) retain();
    }
    explicit CGColorSpace(cf::StringRef name)
        :spaceType(ColorSpace_Named)
    {
        named.name = name->copy();
    }
    explicit CGColorSpace(DeviceSpaceEnum deviceSpace)
        :spaceType(ColorSpace_DeviceDependent)
    {
        device.which = deviceSpace;
    }
    ~CGColorSpace() override {
        if (spaceType == ColorSpace_Named && named.name) {
            named.name->release();
        }
    }
    ColorSpaceEnum getSpaceType() { return spaceType; }
    DeviceSpaceEnum getDeviceSpace();
    cf::StringRef getSpaceName();
};

#endif //__CG_COLORSPACE_H__
