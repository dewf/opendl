//
// Created by dang on 9/24/18.
//

#include "CGColorSpace.h"
#include <assert.h>

CGColorSpace::DeviceSpaceEnum CGColorSpace::getDeviceSpace() {
    assert(spaceType == ColorSpace_DeviceDependent);
    return device.which;
}

cf::StringRef CGColorSpace::getSpaceName() {
    assert(spaceType == ColorSpace_Named);
    return named.name;
}
