#include "CGPath.h"


#include "CGContext.h"

// this is inverted, eventually it's the context that will have a method to add paths
// (we'll append to an existing sub/path)
void CGPath::addToContext(dl_CGContextRef c) {

}

//void CGMutablePath::moveToPoint(const dl_CGAffineTransform * m, dl_CGFloat x, dl_CGFloat y)
//{
//}
//
//void CGMutablePath::addArc(const dl_CGAffineTransform * m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise)
//{
//}
//
//void CGMutablePath::addRelativeArc(const dl_CGAffineTransform * matrix, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta)
//{
//}
//
//void CGMutablePath::addArcToPoint(const dl_CGAffineTransform * m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius)
//{
//}
//
//void CGMutablePath::addCurveToPoint(const dl_CGAffineTransform * m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y)
//{
//}
//
//void CGMutablePath::addLines(const dl_CGAffineTransform * m, const dl_CGPoint * points, size_t count)
//{
//}
//
//void CGMutablePath::addLineToPoint(const dl_CGAffineTransform * m, dl_CGFloat x, dl_CGFloat y)
//{
//}
//
//void CGMutablePath::addPath(const dl_CGAffineTransform * m, dl_CGPathRef path2)
//{
//}
//
//void CGMutablePath::addQuadCurveToPoint(const dl_CGAffineTransform * m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y)
//{
//}
//
//void CGMutablePath::addRect(const dl_CGAffineTransform *m, dl_CGRect rect) {
//	if (m) {
//		throw cf::Exception("transforms not currently supported in CGMutablePath constructor");
//	}
//	PathItem item;
//	item.tag = PathItem::Rect;
//	item.rect.value = rect;
//
//	items.push_back(item);
//}
//
//void CGMutablePath::addRects(const dl_CGAffineTransform * m, const dl_CGRect * rects, size_t count)
//{
//}
//
//void CGMutablePath::addRoundedRect(const dl_CGAffineTransform * transform, dl_CGRect rect, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight)
//{
//}
//
//void CGMutablePath::addEllipseInRect(const dl_CGAffineTransform * m, dl_CGRect rect)
//{
//}
//
//void CGMutablePath::closeSubpath()
//{
//}
