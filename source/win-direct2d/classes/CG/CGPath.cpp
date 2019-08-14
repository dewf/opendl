#include "CGPath.h"

// this is inverted, eventually it's the context that will have a method to add paths
// (we'll append to an existing sub/path)

#include "CGContext.h"

void CGPath::addToContext(dl_CGContextRef c) {
	for (auto i = items.begin(); i != items.end(); i++) {
		switch (i->tag) {
		case PathItem::Rect:
			((CGContextRef)c)->addRect(i->rect.value);
			break;
		case PathItem::Ellipse:
			((CGContextRef)c)->addEllipseInRect(i->ellipse.inRect);
			break;
		case PathItem::RoundedRect:
			((CGContextRef)c)->addRoundedRect(i->rounded.rect, i->rounded.cornerWidth, i->rounded.cornerHeight);
			break;
		}
	}
}
