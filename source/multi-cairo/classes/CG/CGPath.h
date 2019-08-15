//
// Created by dang on 9/25/18.
//

#ifndef __CG_PATH_H__
#define __CG_PATH_H__

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"

#include <vector>

struct PathItem {
    enum Tag {
        Rect,
        Ellipse,
        RoundedRect
    };
    Tag tag;
    union {
        struct {
            dl_CGRect value;
        } rect;
        struct {
            dl_CGRect inRect;
        } ellipse;
        struct {
            dl_CGRect rect;
            dl_CGFloat cornerWidth, cornerHeight;
        } roundedRect;
    };
};

void cairo_ellipse(cairo_t *cr, dl_CGRect &inRect);
void cairo_roundedRect(cairo_t *cr, dl_CGRect &rect, double cornerWidth, double cornerHeight);

class CGPath; typedef CGPath *CGPathRef;
class CGPath : public cf::Object {
protected:
    std::vector<PathItem> items;
public:
    const char *getTypeName() const override {
        return "CGPath";
    }

    CGPathRef copy() override {
        // immutable
        return (CGPathRef) retain();
    }

    CGPath() {}
    explicit CGPath(PathItem &item) {
        items.push_back(item);
    }
    CGPath(const CGPath &other) {
        items = other.items;
    }
    ~CGPath() override {}

    static CGPathRef createWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform);
    static CGPathRef createWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform);
    static CGPathRef createWithRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform);

    dl_CGRect getRect();

    void apply(cairo_t *cr);

    RETAIN_AND_AUTORELEASE(CGPath);
    WEAKREF_MAKE(CGPath);
};


class CGMutablePath; typedef CGMutablePath* CGMutablePathRef;
class CGMutablePath : public CGPath {
public:
    CGMutablePath() : CGPath() {}
    static CGMutablePathRef create() {
        return new CGMutablePath();
    }

    ~CGMutablePath() override {}

    void addRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
        if (m) {
            throw cf::Exception("transforms not currently supported in CGMutablePath constructor");
        }
        PathItem item{};
        item.tag = PathItem::Rect;
        item.rect.value = rect;
        items.push_back(item);
    }

    bool operator <(const Object &b) const override {
        throw cf::Exception("operator < not defined for CGMutablePath");
    }
    CGPathRef copy() override {
        // make an immutable copy
        return new CGPath(*this);
    }

    RETAIN_AND_AUTORELEASE(CGMutablePath)
};

#endif //__CG_PATH_H__
