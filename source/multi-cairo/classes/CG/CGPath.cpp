//
// Created by dang on 9/25/18.
//

#include "CGPath.h"
#include <cmath>

void cairo_arc2(cairo_t *cr, double cx, double cy, double xRadius, double yRadius, double angle1, double angle2)
{
    cairo_matrix_t save_matrix;
    cairo_get_matrix(cr, &save_matrix);

    cairo_translate(cr, cx, cy);

    double widthRatio = 1.0;
    double heightRatio = 1.0;
    double radius;
    if (xRadius > yRadius) {
        heightRatio = yRadius / xRadius;
        radius = xRadius;
    } else {
        widthRatio = xRadius / yRadius;
        radius = yRadius;
    }
    cairo_scale(cr, widthRatio, heightRatio);

    cairo_translate(cr, -cx, -cy);

    cairo_arc(cr, cx, cy, radius, angle1, angle2);

    cairo_set_matrix(cr, &save_matrix);
}

void cairo_ellipse(cairo_t *cr, dl_CGRect &inRect)
{
    double cx = inRect.origin.x + inRect.size.width / 2;
    double cy = inRect.origin.y + inRect.size.height / 2;
    double xRadius = inRect.size.width / 2;
    double yRadius = inRect.size.height / 2;
    cairo_arc2(cr, cx, cy, xRadius, yRadius, 0, M_PI * 2);
}

void cairo_roundedRect(cairo_t *cr, dl_CGRect &rect, double cornerWidth, double cornerHeight)
{
    cairo_new_sub_path(cr);

    double xLeft = rect.origin.x + cornerWidth;
    double xRight = rect.origin.x + rect.size.width - cornerWidth;
    double yTop = rect.origin.y + cornerHeight;
    double yBottom = rect.origin.y + rect.size.height - cornerHeight;

    // top left
    cairo_arc2(cr,
            xLeft, yTop,
            cornerWidth, cornerHeight,
            M_PI, 1.5 * M_PI);

    // top right
    cairo_arc2(cr,
            xRight, yTop,
            cornerWidth, cornerHeight,
            M_PI * 1.5, M_PI * 2.0);

    // bottom right
    cairo_arc2(cr,
            xRight, yBottom,
            cornerWidth, cornerHeight,
            0, M_PI / 2.0);

    // bottom left
    cairo_arc2(cr,
            xLeft, yBottom,
            cornerWidth, cornerHeight,
            M_PI / 2.0, M_PI);

    cairo_close_path(cr);
}

CGPathRef CGPath::createWithRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
    if (transform) throw cf::Exception("CGPathRef::createWithRect - transform param not yet handled");
    PathItem item{};
    item.tag = PathItem::Rect;
    item.rect.value = rect;
    return new CGPath(item);
}

CGPathRef CGPath::createWithEllipseInRect(dl_CGRect rect, const dl_CGAffineTransform *transform) {
    if (transform) throw cf::Exception("CGPathRef::createWithEllipseInRect - transform param not yet handled");
    PathItem item{};
    item.tag = PathItem::Ellipse;
    item.ellipse.inRect = rect;
    return new CGPath(item);
}

CGPathRef CGPath::createWithRoundedRect(dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight,
                                        const dl_CGAffineTransform *transform) {
    if (transform) throw cf::Exception("CGPathRef::createWithRoundedRect - transform param not yet handled");
    PathItem item{};
    item.tag = PathItem::RoundedRect;
    item.roundedRect.rect = rect;
    item.roundedRect.cornerWidth = cornerWidth;
    item.roundedRect.cornerHeight = cornerHeight;
    return new CGPath(item);
}

dl_CGRect CGPath::getRect() {
    if (items.size() == 1 && items[0].tag == PathItem::Rect) {
        return items[0].rect.value;
    } else {
        throw cf::Exception("CGPath is not a rect");
    }
}

void CGPath::apply(cairo_t *cr) {
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
