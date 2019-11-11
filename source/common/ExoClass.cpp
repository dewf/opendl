//
//  dlExoClass.cpp
//  opendl
//
//  Created by Daniel X on 9/6/18.
//  Copyright (c) 2018 OpenDL developers. All rights reserved.
//

#include "ExoClass.h"

#include <map>
#include <assert.h>

// associated data dict ==========
// (this exists since it's easier than creating a real CF type, as documented here: https://opensource.apple.com/source/CF/CF-635/CFRuntime.h )
// ie, we wanted to piggyback some data on things like CTFrameSetter, without interfering in normal operation (so CFRelease would still work, etc)
// maybe should rewrite with something like NSMapTable to be more Obj-C-friendly?
static std::map<size_t, void *> assocMap;

static void *getAssociatedData(const void *keyPtr) {
    auto key = (size_t)keyPtr;
    auto found = assocMap.find(key);
    if (found != assocMap.end()) {
        return found->second;
    } else {
        return NULL;
    }
}
static void setAssociatedData(void *keyPtr, void *data) {
    auto key = (size_t)keyPtr;
    assocMap.insert(std::map<size_t, void *>::value_type(key, data));
}
static void *removeAssociatedData(void *keyPtr) {
    auto key = (size_t)keyPtr;
    auto removed = assocMap[key];
    assocMap.erase(key);
    return removed;
}

ExoClass::ExoClass(void *key)
    :assocKey(key)
{
    setAssociatedData(key, this);
}
ExoClass::~ExoClass()
{
    removeAssociatedData(assocKey);
}
ExoClass *ExoClass::getFor(const void *key)
{
    return (ExoClass *)getAssociatedData(key);
}

void ExoClass::retain() {
    refCount++;
}
void ExoClass::release() {
    refCount--;
    assert(refCount >= 0);
    if (refCount == 0) {
        delete this;
    }
}
