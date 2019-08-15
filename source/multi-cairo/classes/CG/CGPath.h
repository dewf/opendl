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
    std::vector<PathItem> items;
public:
    const char *getTypeName() const override {
        return "CGPath";
    }

    CGPathRef copy() override {
        // immutable
        return (CGPathRef) retain();
    }

    explicit CGPath(PathItem &item) {
        items.push_back(item);
    }
    static CGPathRef createWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
        if (transform) throw cf::Exception("CGPathRef::createWithRect - transform param not yet handled");
        PathItem item{};
        item.tag = PathItem::Rect;
        item.rect.value = rect;
        return new CGPath(item);
    }
    static CGPathRef createWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
        if (transform) throw cf::Exception("CGPathRef::createWithEllipseInRect - transform param not yet handled");
        PathItem item{};
        item.tag = PathItem::Ellipse;
        item.ellipse.inRect = rect;
        return new CGPath(item);
    }
    static CGPathRef createWithRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform) {
        if (transform) throw cf::Exception("CGPathRef::createWithRoundedRect - transform param not yet handled");
        PathItem item{};
        item.tag = PathItem::RoundedRect;
        item.roundedRect.rect = rect;
        item.roundedRect.cornerWidth = cornerWidth;
        item.roundedRect.cornerHeight = cornerHeight;
        return new CGPath(item);
    }

    ~CGPath() override {
        // nothing yet
    }

    dl_CGRect getRect() {
        if (items.size() == 1 && items[0].tag == PathItem::Rect) {
            return items[0].rect.value;
        } else {
            throw cf::Exception("CGPath is not a rect");
        }
    }

    void apply(cairo_t *cr) {
        for (auto &item : items) {
            switch (item.tag) {
                case PathItem::Rect: {
                    dl_CGRect rect = item.rect.value;
                    cairo_rectangle(cr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
                    break;
                }
                case PathItem::Ellipse: {
                    cairo_ellipse(cr, item.ellipse.inRect);
                    break;
                }
                case PathItem::RoundedRect: {
                    cairo_roundedRect(cr, item.roundedRect.rect, item.roundedRect.cornerWidth, item.roundedRect.cornerHeight);
                    break;
                }
                default:
                    throw cf::Exception("CGPath::apply() - unhandled path type");
            }
        }
    }

    RETAIN_AND_AUTORELEASE(CGPath);
    WEAKREF_MAKE(CGPath);
};

#endif //__CG_PATH_H__
