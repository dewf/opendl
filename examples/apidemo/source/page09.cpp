#include "page09.h"

#include <stdio.h>
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
    dl_CGPathAddArc(path, m, center.x, center.y, radius + 70.71067, -(M_PI/4), 0, true);
    
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
