//
//  dlExoClass.h
//  opendl
//
//  Created by Daniel X on 9/6/18.
//  Copyright (c) 2018 OpenDL developers. All rights reserved.
//

#ifndef __opendl__dlExoClass__
#define __opendl__dlExoClass__

// an "ExoClass" to piggyback data on native CF/CG types (because making new CF classes is both difficult and frowned upon by Apple)
struct ExoClass {
    void *assocKey = nullptr;
    // ===========
    ExoClass(void *key);
    virtual ~ExoClass();
    static ExoClass *getFor(const void *key);
};

#endif /* defined(__opendl__dlExoClass__) */
