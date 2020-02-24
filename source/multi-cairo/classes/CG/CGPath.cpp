//
// Created by dang on 9/25/18.
//

#include "CGPath.h"
#include "../../../common/pathstuff.h"
#include "../../../common/geometry.h"
#include "../../util.h"
#include <cmath>

static inline void addArc(cairo_t *cr, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, int clockwise, dl_CGPoint &lastPoint)
{
    if (clockwise) {
        // end must be greater than start, otherwise cairo alters the end (by repeatedly adding 2PI)
        if (startAngle > endAngle) {
            std::swap(startAngle, endAngle);
        }
        cairo_arc(cr, x, y, radius, startAngle, endAngle);
    } else {
        // start must be greater than end, otherwise cairo alters the end (by repeatedly subtracting 2PI)
        if (startAngle < endAngle) {
            std::swap(startAngle, endAngle);
        }
        cairo_arc_negative(cr, x, y, radius, startAngle, endAngle);
    }
    cairo_get_current_point(cr, &lastPoint.x, &lastPoint.y);
}

static inline void addArcToPoint(cairo_t *cr, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius)
{
    // draw from current point to (almost) x1,y1 - create arc of specified radius there, tangent to line from x1,y1 to x2,y2
    // find angle between two lines:
    double x0, y0; // current point
    cairo_get_current_point(cr, &x0, &y0);

    double centerX, centerY, startX, startY, startAngle, endAngle;
    calcArcToPoint(x0, y0, x1, y1, x2, y2, radius, &centerX, &centerY, &startX, &startY, &startAngle, &endAngle);

    cairo_line_to(cr, startX, startY);
    cairo_arc(cr, centerX, centerY, radius, startAngle, endAngle);
    // we end at the end of that arc, resume drawing from there
}

//static inline void checkBeginFigure(cairo_t *cr, bool &figureOpen, dl_CGPoint &figureBeginPoint, dl_CGPoint &lastPoint) {
//    if (!figureOpen) {
//        cairo_new_sub_path(cr);
//        figureBeginPoint = lastPoint;
//        figureOpen = true;
//    }
//}

static inline void writeSegment(PathSegment &seg, cairo_t *cr, bool &figureOpen, dl_CGPoint &figureBeginPoint, dl_CGPoint &lastPoint) {
    switch (seg.tag) {
        case PathSegment::Tag_MoveToPoint: {
            cairo_move_to(cr, seg.point.x, seg.point.y);
            figureBeginPoint = dl_CGPointMake(seg.point.x, seg.point.y);
            lastPoint = figureBeginPoint;
            break;
        }
        case PathSegment::Tag_LineToPoint: {
            cairo_line_to(cr, seg.point.x, seg.point.y);
            lastPoint = dl_CGPointMake(seg.point.x, seg.point.y);
            break;
        }
        case PathSegment::Tag_Arc: {
            addArc(cr, seg.arc.x, seg.arc.y, seg.arc.radius, seg.arc.startAngle, seg.arc.endAngle, seg.arc.clockwise, lastPoint);
            break;
        }
        case PathSegment::Tag_ArcToPoint: {
            addArcToPoint(cr, seg.arcToPoint.x1, seg.arcToPoint.y1, seg.arcToPoint.x2, seg.arcToPoint.y2, seg.arcToPoint.radius);
            lastPoint = dl_CGPointMake(seg.arcToPoint.x2, seg.arcToPoint.y2);
            break;
        }
        case PathSegment::Tag_CurveToPoint: {
            cairo_curve_to(cr,
                           seg.curveToPoint.cp1x, seg.curveToPoint.cp1y,
                           seg.curveToPoint.cp2x, seg.curveToPoint.cp2y,
                           seg.curveToPoint.x, seg.curveToPoint.y);
            lastPoint = dl_CGPointMake(seg.curveToPoint.x, seg.curveToPoint.y);
            break;
        }
        case PathSegment::Tag_QuadCurveToPoint: {
            auto cp1x = lastPoint.x + 2/3.0 * (seg.quadCurveToPoint.cpx - lastPoint.x);
            auto cp1y = lastPoint.y + 2/3.0 * (seg.quadCurveToPoint.cpy - lastPoint.y);
            auto cp2x = seg.quadCurveToPoint.x + 2/3.0 * (seg.quadCurveToPoint.cpx - seg.quadCurveToPoint.x);
            auto cp2y = seg.quadCurveToPoint.y + 2/3.0 * (seg.quadCurveToPoint.cpy - seg.quadCurveToPoint.y);
            cairo_curve_to(cr,
                    cp1x, cp1y,
                    cp2x, cp2y,
                    seg.quadCurveToPoint.x, seg.quadCurveToPoint.y);
            lastPoint = dl_CGPointMake(seg.quadCurveToPoint.x, seg.quadCurveToPoint.y);
            break;
        }
        case PathSegment::Tag_Closure: {
            cairo_close_path(cr);
            break;
        }
        default:
            printf("writeSegment: unhandled switch case\n");
            assert(false);
    } // end switch
}

static void cairo_segmented(cairo_t *cr, std::vector<PathSegment> &segments) {
    bool figureOpen = false;
    dl_CGPoint figureBeginPoint, lastPoint;

    for (auto &seg : segments) {
        writeSegment(seg, cr, figureOpen, figureBeginPoint, lastPoint);
    }

    if (figureOpen) {
        // anything necessary to "end" open paths?
    }
}

static void cairo_arc2(cairo_t *cr, double cx, double cy, double xRadius, double yRadius, double angle1, double angle2)
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

static void cairo_ellipse(cairo_t *cr, dl_CGRect &inRect)
{
    double cx = inRect.origin.x + inRect.size.width / 2;
    double cy = inRect.origin.y + inRect.size.height / 2;
    double xRadius = inRect.size.width / 2;
    double yRadius = inRect.size.height / 2;
    cairo_arc2(cr, cx, cy, xRadius, yRadius, 0, M_PI * 2);
}

static void cairo_roundedRect(cairo_t *cr, dl_CGRect &rect, double cornerWidth, double cornerHeight)
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

void renderPath(SubPath &sp, cairo_t *cr) {
    cairo_matrix_t m;
    if (sp.hasTransform) {
        cairo_get_matrix(cr, &m);
        auto m2 = dl_to_cairo_matrix(sp.transform);
        cairo_set_matrix(cr, &m2);
    }
    cairo_new_sub_path(cr);
    switch (sp.tag) {
        case SubPath::Tag_Rect: {
            auto &r = sp.rect.value;
            cairo_rectangle(cr, r.origin.x, r.origin.y, r.size.width, r.size.height);
            break;
        }
        case SubPath::Tag_RoundedRect:
            cairo_roundedRect(cr, sp.rounded.rect, sp.rounded.cornerWidth, sp.rounded.cornerHeight);
            break;
        case SubPath::Tag_Ellipse:
            cairo_ellipse(cr, sp.ellipse.inRect);
            break;
        case SubPath::Tag_Segmented:
            cairo_segmented(cr, sp.segments);
            break;
    }
    if (sp.hasTransform) {
        cairo_set_matrix(cr, &m);
    }
}
