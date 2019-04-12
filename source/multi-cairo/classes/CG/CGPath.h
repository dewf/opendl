//
// Created by dang on 9/25/18.
//

#ifndef __CG_PATH_H__
#define __CG_PATH_H__

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

class CGPath;

typedef CGPath *CGPathRef;

class CGPath : public cf::Object {
    enum PathTypeEnum {
        PathType_Rect,
    };
    PathTypeEnum pathType;
    union {
        dl_CGRect rect;
    };
public:
    const char *getTypeName() const override {
        return "CGPath";
    }

    CGPathRef copy() override {
        // immutable
        return (CGPathRef) retain();
    }

    // public API methods
    CGPath(dl_CGRect rect)
            : rect(rect), pathType(PathType_Rect) {
    }

    ~CGPath() {
        // nothing yet
    }

    dl_CGRect getRect() {
        if (pathType == PathType_Rect) {
            return rect;
        } else {
            throw cf::Exception("CGPath is not a rect");
        }
    }

    void apply(cairo_t *cr) {
        switch(pathType) {
            case PathType_Rect:
                cairo_rectangle(cr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
                break;
            default:
                throw cf::Exception("CGPath::apply() - unhandled path type");
        }
    }

    RETAIN_AND_AUTORELEASE(CGPath);
    WEAKREF_MAKE(CGPath);
};

#endif //__CG_PATH_H__
