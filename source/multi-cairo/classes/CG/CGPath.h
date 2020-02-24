//
// Created by dang on 9/25/18.
//

#ifndef __CG_PATH_H__
#define __CG_PATH_H__

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"

#include <vector>
#include <cmath>

#include "../../../common/pathstuff.h"

void renderPath(SubPath &sp, cairo_t *cr);

// CGPath / CGMutablePath proper =========================================================================

class CGPath; typedef CGPath* CGPathRef;
class CGPath : public cf::Object {
protected:
    std::vector<SubPath> subPaths;

    SubPath *currentSub() {
        return &subPaths[subPaths.size() - 1];
    }

public:
    const char *getTypeName() const override { return "CGPath"; }

    CGPath() {}

    CGPath(SubPath sub) {
        subPaths.push_back(sub);
        // todo: update currentPoint with end of SubPath
    }

    CGPath(const CGPath &other) {
        subPaths = other.subPaths;
        // todo: update currentPoint with end of last SubPath
    }

    static CGPathRef createWithRect(dl_CGRect &rect, const dl_CGAffineTransform *transform) {
        return new CGPath(SubPath::createRect(rect, transform));
    }

    static CGPathRef createWithEllipseInRect(dl_CGRect &rect, const dl_CGAffineTransform *transform) {
        return new CGPath(SubPath::createEllipse(rect, transform));
    }

    static CGPathRef createWithRoundedRect(dl_CGRect &rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *transform) {
        return new CGPath(SubPath::createRoundedRect(rect, cornerWidth, cornerHeight, transform));
    }

    bool isRect() {
        // kind of hacky at the moment, no mechanism in place to prevent adding further subpaths
        return
                subPaths.size() == 1 &&
                currentSub()->isRect();
    }

    dl_CGRect getRect() {
        if (isRect()) {
            return currentSub()->getRect();
        }
        else {
            throw cf::Exception("CGPath was not a rect");
        }
    }

    virtual CGPathRef copy() {
        // immutable so return self
        return (CGPathRef)retain();
    }

    // for CGMutablePaths accessing needing access to another CGPath's content
    std::vector<SubPath>& getSubPaths() {
        return subPaths;
    }

    void sendToContext(cairo_t *cr) {
        for (auto &sp : subPaths) {
            renderPath(sp, cr);
        }
    }

    dl_CGPoint getCurrentPoint() {
        return currentSub()->endPoint;
    }

    RETAIN_AND_AUTORELEASE(CGPath)
};


class CGMutablePath; typedef CGMutablePath* CGMutablePathRef;
class CGMutablePath : public CGPath {
private:
    bool open = false; // is there a segmented path on the stack, ready to go?

    inline SubPath *mutableSub() {
        if (!open) {
            subPaths.push_back(SubPath::createEmptyPath());
            open = true;
        }
        return currentSub();
    }
public:
    CGMutablePath() : CGPath() {}

    CGMutablePath(const CGPath *other) // aka create mutable copy
            : CGPath(*other) // copy all previous subpaths
    {}

    ~CGMutablePath() {}

    virtual bool operator <(const Object &b) const {
        throw cf::Exception("operator < not defined for CGMutablePath");
    }
    virtual CGPathRef copy() {
        // make an immutable copy
        return new CGPath(*this);
    }

    void moveToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
        mutableSub()->moveToPoint(m, x, y);
    }
    void addArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise) {
        mutableSub()->addArc(m, x, y, radius, startAngle, endAngle, clockwise);
    }
    void addRelativeArc(const dl_CGAffineTransform *matrix, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta) {
        //mutableSub()->addRelativeArc(matrix, x, y, radius, startAngle, delta);
        bool clockwise;
        dl_CGFloat endAngle;
        if (delta >= 0) {
            clockwise = true;
            endAngle = fmod(startAngle + delta, M_PI * 2.0);
        }
        else {
            clockwise = false;
            endAngle = fmod(startAngle + M_PI * 2.0 - delta, M_PI * 2.0);
        }
        mutableSub()->addArc(matrix, x, y, radius, startAngle, endAngle, clockwise);
    }
    void addArcToPoint(const dl_CGAffineTransform *m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius) {
        mutableSub()->addArcToPoint(m, x1, y1, x2, y2, radius);
    }
    void addCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y) {
        mutableSub()->addCurveToPoint(m, cp1x, cp1y, cp2x, cp2y, x, y);
    }
    void addLineToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
        mutableSub()->addLineToPoint(m, x, y);
    }
    void addLines(const dl_CGAffineTransform *m, const dl_CGPoint *points, size_t count) {
        mutableSub()->addLines(m, points, count);
    }
    void addQuadCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y) {
        mutableSub()->addQuadCurveToPoint(m, cpx, cpy, x, y);
    }
    void addRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
        open = false;
        subPaths.push_back(SubPath::createRect(rect, m));
    }
    void addRects(const dl_CGAffineTransform *m, const dl_CGRect *rects, size_t count) {
        open = false;
        for (size_t i = 0; i < count; i++) {
            subPaths.push_back(SubPath::createRect(rects[i], m));
        }
    }
    void addRoundedRect(const dl_CGAffineTransform *transform, dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight) {
        open = false;
        subPaths.push_back(SubPath::createRoundedRect(rect, cornerWidth, cornerHeight, transform));
    }
    void addEllipseInRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
        open = false;
        subPaths.push_back(SubPath::createEllipse(rect, m));
        // TODO: currentpoint should be ... ?
    }
    void addPath(const dl_CGAffineTransform *m, CGPathRef path2) {
        open = false;
        auto sourceSubs = path2->getSubPaths();
        for (auto & sourceSub : sourceSubs) {
            SubPath sp = sourceSub;
            if (m) {
                sp.concatTransform(*m);
            }
            subPaths.push_back(std::move(sp));
        }
    }
    void closeSubpath() {
        mutableSub()->close();
        open = false; // will need to push a new segmented subpath to add further segments (mutableSub() takes care of it)
    }
    void reset() {
        subPaths.clear();
        open = false;
    }

    RETAIN_AND_AUTORELEASE(CGMutablePath)
};

#endif //__CG_PATH_H__
