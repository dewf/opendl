#include "page09.h"

#include <stdio.h>

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

void CPage09::drawPoint(const dl_CGContextRef &context, dl_CGPoint &p, dl_CGFloat radius)
{
	dl_CGContextBeginPath(context);
	dl_CGContextAddArc(context, p.x, p.y, radius, 0, degreesToRadians(360), 0);
	dl_CGContextFillPath(context);
	//auto r = dl_CGRectMake(p.x - radius, p.y - radius, radius * 2, radius * 2);
	//dl_CGContextFillRect(context, r);
}

void CPage09::render(dl_CGContextRef context, int width, int height)
{
	dl_CGContextSetRGBFillColor(context, 0, 0, 0, 1);
	dl_CGContextFillRect(context, dl_CGRectMake(0, 0, width, height));

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
