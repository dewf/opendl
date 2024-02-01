#include "page09.h"

#include <math.h>

#include "common.h"

CPage09::CPage09(WindowFuncs * parent)
	:PageCommon(parent) {}

CPage09::~CPage09()
{
	dl_CGGradientRelease(grad);
    dl_CGGradientRelease(grad2);
}

void CPage09::init()
{
	auto space = dl_CGColorSpaceCreateWithName(dl_kCGColorSpaceGenericRGB);
	dl_CGFloat components[8] = {
		0.0, 0.5, 1.0, 1.0,
		0.0, 1.0, 0.0, 1.0
	};
	dl_CGFloat locations[2] = { 0.0, 1.0 };
	grad = dl_CGGradientCreateWithColorComponents(space, components, locations, 2);
    
    // change the alpha ...
    components[3] = 0.3;
    components[7] = 0.3;
    grad2 = dl_CGGradientCreateWithColorComponents(space, components, locations, 2);

	dl_CGColorSpaceRelease(space);
}

dl_CGFloat degreesToRadians(dl_CGFloat x) {
	return (x * M_PI) / 180.0;
}

void CPage09::drawPoint(dl_CGContextRef context, dl_CGPoint &p, dl_CGFloat radius)
{
	dl_CGContextBeginPath(context);
	dl_CGContextAddArc(context, p.x, p.y, radius, 0, degreesToRadians(360), 0);
	dl_CGContextFillPath(context);
	//auto r = dl_CGRectMake(p.x - radius, p.y - radius, radius * 2, radius * 2);
	//dl_CGContextFillRect(context, r);
}

void CPage09::transformTest(dl_CGContextRef context) {
    auto r = dl_CGRectMake(100, 100, 500, 500);
    auto start = dl_CGPointMake(100, 100);
    auto end = dl_CGPointMake(100, 600);

    dl_CGContextSetRGBFillColor(context, 1, 0, 0, 1);
    drawPoint(context, start, 5);
    drawPoint(context, end, 5);
    
    dl_CGContextSaveGState(context);

    // draw semi-transparent (darker against black) one first
    dl_CGContextDrawLinearGradient(context, grad2, start, end, 0);
    
    // original rect
    dl_CGContextSetRGBStrokeColor(context, 1, 1, 0, 1); // yellow
    dl_CGContextStrokeRectWithWidth(context, r, 2);

    // transform
    dl_CGContextTranslateCTM(context, 256.0, 0);
    dl_CGContextScaleCTM(context, 0.25, 0.5);
    dl_CGContextRotateCTM(context, degreesToRadians(22));

    // start/end, transformed
    dl_CGContextSetRGBFillColor(context, 1, 0, 0, 1);
    drawPoint(context, start, 5);
    drawPoint(context, end, 5);

    // rect, transformed
    dl_CGContextSetRGBStrokeColor(context, 1, 1, 0, 1); // yellow
    dl_CGContextStrokeRectWithWidth(context, r, 2);
    
    // semi-transparent again, post-transform
    dl_CGContextDrawLinearGradient(context, grad2, start, end, 0);

    // rect-clipped, full bright version
    dl_CGContextClipToRect(context, r);
    dl_CGContextDrawLinearGradient(context, grad, start, end, 0);
    
    dl_CGContextRestoreGState(context);
}

void triangleThing(dl_CGContextRef context, dl_CGPoint center, dl_CGAffineTransform *m, dl_CGFloat baseOpacity) {
    dl_CGContextSaveGState(context);

    // triangle thing from page 01, as a mutable path
    
    // center 273.33, 343.33
    //dl_CGPoint points[] = { {150, 150}, {540, 340}, {130, 540}};
    dl_CGPoint points[] = {
        {center.x - 123.33, center.y - 193.33},
        {center.x + 266.67, center.y + 3.33 },
        {center.x - 143.33, center.y + 196.67}
    };
    
    // line version
    auto linePath = dl_CGPathCreateMutable();
    dl_CGPathMoveToPoint(linePath, m, points[0].x, points[0].y);
    
    dl_CGPathAddLineToPoint(linePath, m, points[1].x, points[1].y);
    dl_CGPathAddLineToPoint(linePath, m, points[2].x, points[2].y);
    dl_CGPathCloseSubpath(linePath);
    //
    dl_CGContextSetRGBStrokeColor(context, 1, 1, 1, baseOpacity * 0.5);
    dl_CGContextSetLineWidth(context, 2.0);
    dl_CGFloat dashLengths[2] = { 3.0, 3.0 };
    dl_CGContextSetLineDash(context, 0.0, dashLengths, 2);
    dl_CGContextAddPath(context, linePath);
    dl_CGContextStrokePath(context);
    
    dl_CGPathRelease(linePath);
    dl_CGContextSetLineDash(context, 0, nullptr, 0); // clear dashes
    
    // curved version
    auto curvedPath = dl_CGPathCreateMutable();
    dl_CGPoint startEnd = betweenPoints(points[2], points[0]);
    
    dl_CGPathMoveToPoint(curvedPath, m, startEnd.x, startEnd.y);
    dl_CGPathAddArcToPoint(curvedPath, m, points[0].x, points[0].y, points[1].x, points[1].y, 20.0);
    dl_CGPathAddArcToPoint(curvedPath, m, points[1].x, points[1].y, points[2].x, points[2].y, 20.0);
    dl_CGPathAddArcToPoint(curvedPath, m, points[2].x, points[2].y, points[0].x, points[0].y, 20.0);
    dl_CGPathCloseSubpath(curvedPath);
    //
    dl_CGContextSetRGBStrokeColor(context, 0.85, 0, 1, baseOpacity * 0.75);
    dl_CGContextSetLineWidth(context, 4.0);
    dl_CGContextAddPath(context, curvedPath);
    dl_CGContextStrokePath(context);
    
    dl_CGPathRelease(curvedPath);
    
    // restore
    dl_CGContextRestoreGState(context);
}

void miscShapes(dl_CGContextRef context, dl_CGPoint center, dl_CGAffineTransform *m, dl_CGFloat baseOpacity) {
    
    dl_CGContextSetLineWidth(context, 2.0f);
    
    const int POINTS_SIDE = 230;
    dl_CGPoint points[] = {
        { center.x - POINTS_SIDE/2, center.y - POINTS_SIDE/2 },
        { center.x + POINTS_SIDE/2, center.y - POINTS_SIDE/2 },
        { center.x + POINTS_SIDE/2, center.y + POINTS_SIDE/2 },
        { center.x - POINTS_SIDE/2, center.y + POINTS_SIDE/2 },
        
        // extra control points for final segment
        { center.x - POINTS_SIDE/2 - 100.0, center.y + POINTS_SIDE/2 - 20.0 },
        { center.x - POINTS_SIDE/2, center.y - POINTS_SIDE/2 + 30.0 }
    };

    const int ELLIPSE_WIDTH = 200.0;
    const int ELLIPSE_HEIGHT = 100.0;
    auto r = dl_CGRectMake(center.x - ELLIPSE_WIDTH/2, center.y - ELLIPSE_HEIGHT/2, ELLIPSE_WIDTH, ELLIPSE_HEIGHT);
    
    // black background
    const int EXTRA = 4.0;
    auto r2 = dl_CGRectMake(center.x - 300.0 - EXTRA, center.y - POINTS_SIDE/2 - EXTRA, 600.0 + EXTRA*2, POINTS_SIDE + EXTRA*2);
    auto bgRect = dl_CGPathCreateWithRect(r2, m);
    dl_CGContextSetRGBFillColor(context, 0, 0, 0, baseOpacity);
    dl_CGContextAddPath(context, bgRect);
    dl_CGContextFillPath(context);

    auto rounded = dl_CGPathCreateWithRoundedRect(r, 10.0, 10.0, m);
    auto mutable01 = dl_CGPathCreateMutableCopy(rounded);

    auto r_square = dl_CGRectMake(center.x - ELLIPSE_WIDTH / 8, center.y - ELLIPSE_WIDTH / 8, ELLIPSE_WIDTH / 4, ELLIPSE_WIDTH / 4);
    auto square = dl_CGPathCreateWithRect(r_square, nullptr); // no transform on the square itself, being added to path with xform
    dl_CGPathAddPath(mutable01, m, square);
    
    dl_CGContextSetRGBStrokeColor(context, 0.3, 0, 1, baseOpacity);
    dl_CGContextAddPath(context, mutable01);
    dl_CGContextStrokePath(context);

    auto ellipse = dl_CGPathCreateWithEllipseInRect(r, m);
    auto mutable02 = dl_CGPathCreateMutableCopy(ellipse);
    
    dl_CGPathAddLines(mutable02, m, points, 4); // already includes implicit moveToPoint(point[0])
    dl_CGPathAddCurveToPoint(mutable02, m, points[4].x, points[4].y, points[5].x, points[5].y, points[0].x, points[0].y);
    // closing the path looks a little nicer
    dl_CGPathCloseSubpath(mutable02);
    
    dl_CGContextSetRGBStrokeColor(context, 1, 0, 0.3, baseOpacity);
    dl_CGContextAddPath(context, mutable02);
    dl_CGFloat dashes[] = { 3.0, 3.0 };
    dl_CGContextSetLineDash(context, 0, dashes, 2);
    dl_CGContextStrokePath(context);
    
    // clear dashes
    dl_CGContextSetLineDash(context, 0, nullptr, 0);

    // quad curves
    auto mutable03 = dl_CGPathCreateMutable();
    dl_CGPathMoveToPoint(mutable03, m, center.x - 300.0, center.y);
    dl_CGPathAddQuadCurveToPoint(mutable03, m, center.x - 150, center.y - 150, center.x, center.y);
    dl_CGPathAddQuadCurveToPoint(mutable03, m, center.x + 150, center.y + 150, center.x + 300, center.y);
    
    dl_CGContextAddPath(context, mutable03);
    dl_CGContextSetRGBStrokeColor(context, 1, 1, 1, baseOpacity);
    dl_CGContextStrokePath(context);
    
    // release
    dl_CGPathRelease(mutable01);
    dl_CGPathRelease(rounded);
    dl_CGPathRelease(square);
    
    dl_CGPathRelease(mutable02);
    dl_CGPathRelease(ellipse);
    
    dl_CGPathRelease(mutable03);
}

void subPaths(dl_CGContextRef context, dl_CGPoint center, const dl_CGAffineTransform *m, dl_CGFloat baseOpacity) {
    // final misc shapes
    dl_CGRect rects[] = {
        dl_CGRectMake(center.x + 20, center.y - 90, 40.0, 40.0),
        dl_CGRectMake(center.x - 70, center.y, 60.0, 60.0)
    };
    auto mutable04 = dl_CGPathCreateMutable();
    
    auto r1 = dl_CGRectMake(center.x - 80.0, center.y - 80.0, 40.0, 40.0);
    dl_CGPathAddRect(mutable04, m, r1);
    dl_CGPathAddRects(mutable04, m, rects, 2);
    
    auto r2 = dl_CGRectMake(center.x + 60, center.y + 20, 60.0, 60.0);
    dl_CGPathAddRoundedRect(mutable04, m, r2, 10.0, 10.0);
    
    auto r3 = dl_CGRectMake(center.x - 40, center.y - 40, 80.0, 80.0);
    dl_CGPathAddEllipseInRect(mutable04, m, r3);
    
    // black outer path
    dl_CGContextAddPath(context, mutable04);
    dl_CGContextSetRGBStrokeColor(context, 0, 0, 0, baseOpacity);
    dl_CGContextSetLineWidth(context, 6.5);
    dl_CGContextStrokePath(context);

    // then cyan
    dl_CGContextAddPath(context, mutable04);
    dl_CGContextSetRGBStrokeColor(context, 0, 0.8, 1, baseOpacity);
    dl_CGContextSetLineWidth(context, 2.5);
    dl_CGContextStrokePath(context);
    
    dl_CGPathRelease(mutable04);
}

dl_CGAffineTransform makeTransform(dl_CGPoint p, dl_CGFloat angle, dl_CGFloat scale) {
    auto m1 = dl_CGAffineTransformTranslate(dl_CGAffineTransformIdentity, p.x, p.y);
    auto m2 = dl_CGAffineTransformRotate(m1, angle);
    auto m3 = dl_CGAffineTransformScale(m2, scale, scale);
    auto m4 = dl_CGAffineTransformTranslate(m3, -p.x, -p.y);
    return m4;
}

void keyholeThing(dl_CGContextRef context, dl_CGPoint center, dl_CGAffineTransform *m, dl_CGFloat colorInterp) {
    auto path = dl_CGPathCreateMutable();
    
    auto radius = 50.0;
    dl_CGPathMoveToPoint(path, m, center.x + radius, center.y);
    dl_CGPathAddArc(path, m, center.x, center.y, radius, 0, M_PI * 2.0 - (M_PI/4), true);
//    dl_CGPathAddArc(path, m, center.x, center.y, radius + 70.71067, -(M_PI/4), 0, true);
    dl_CGPathAddRelativeArc(path, m, center.x, center.y, radius + 70.71067, -(M_PI/4), M_PI/4);
    
    // just a dot to test GetCurrentPoint
    auto p1 = dl_CGPathGetCurrentPoint(path);

    dl_CGPathCloseSubpath(path);
    
    // draw a dot at p1 (underneath path)
    dl_CGContextSetRGBFillColor(context, 1, 1, 0, 1);
    dl_CGContextAddArc(context, p1.x, p1.y, 5.0, 0, M_PI*2.0, true);
    dl_CGContextFillPath(context);

    // draw path itself
    dl_CGContextSetRGBStrokeColor(context, 1 * colorInterp, 0, 0, 1);
    dl_CGContextSetLineWidth(context, 3.0);
    dl_CGContextAddPath(context, path);
    dl_CGContextStrokePath(context);
    
    dl_CGPathRelease(path);
}

void CPage09::mutablePathTest(dl_CGContextRef context)
{
    dl_CGContextSaveGState(context);
    
    const int NUM_STEPS = 5;

    auto center = dl_CGPointMake(750, 350);
    
    // complete subpaths (rect, ellipse, etc)
    auto subPathsCenter = center; //dl_CGPointMake(200, 600);
    auto m3 = makeTransform(subPathsCenter, M_PI/10, 2.0);
    subPaths(context, subPathsCenter, &m3, 0.5);
    subPaths(context, subPathsCenter, nullptr, 1.0);

    // keyhole
    for (int i=NUM_STEPS; i >= 0; i--) {
        auto scale = 1.0 + ((i * 1.0) / NUM_STEPS);
        auto rot = (i * (M_PI/6)) / NUM_STEPS;
        auto color = 1.0 - (i / (double)NUM_STEPS);
        auto m = makeTransform(center, rot, scale);
        keyholeThing(context, center, &m, color);
    }
    
    // triangle w/ arc-to-point test
    auto rotCenter = dl_CGPointMake(273.33, 343.33);
    for (int i=0; i< NUM_STEPS + 1; i++) {
        auto angle = (i * (M_PI / 4.0)) / NUM_STEPS;
        auto opacity = 1.0 - (i / (double)NUM_STEPS);
        auto scale = 1.0 - ((i * 0.5) / NUM_STEPS);
        auto m = makeTransform(rotCenter, angle, scale);
        triangleThing(context, rotCenter, &m, opacity);
    }
    
    auto shapesCenter = dl_CGPointMake(600, 600);
    auto m2 = makeTransform(shapesCenter, M_PI/9.0, 0.5);
    miscShapes(context, shapesCenter, &m2, 1.0);
    
    
    // restore
    dl_CGContextRestoreGState(context);
}

void CPage09::render(dl_CGContextRef context, int width, int height)
{
	dl_CGContextSetRGBFillColor(context, 0, 0, 0, 1);
	dl_CGContextFillRect(context, dl_CGRectMake(0, 0, width, height));
    
    transformTest(context);
    mutablePathTest(context);
}
