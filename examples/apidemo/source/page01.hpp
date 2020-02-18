#ifndef __PAGE01_HPP__
#define __PAGE01_HPP__

#include "common.h"

void drawStrokedLine(dl_CGContextRef context, dl_CGPoint start, dl_CGPoint end) {
	dl_CGContextBeginPath(context);
	dl_CGContextMoveToPoint(context, start.x, start.y);
	dl_CGContextAddLineToPoint(context, end.x, end.y);
	dl_CGContextDrawPath(context, dl_kCGPathStroke);
}
void doBlueRect(dl_CGContextRef context, dl_CGRect r, dl_CGFloat alpha) {
	dl_CGContextSetRGBStrokeColor(context, 1, 1, 0, alpha);
	dl_CGContextSetRGBFillColor(context, 0, 0, 1, alpha);
	dl_CGContextStrokeRectWithWidth(context, r, 2);
	dl_CGContextFillRect(context, r);
}
void createRectPath(dl_CGContextRef context, dl_CGRect rect) {
	dl_CGContextBeginPath(context);
	dl_CGContextMoveToPoint(context, rect.origin.x, rect.origin.y);
	dl_CGContextAddLineToPoint(context, rect.origin.x + rect.size.width, rect.origin.y);
	dl_CGContextAddLineToPoint(context, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
	dl_CGContextAddLineToPoint(context, rect.origin.x, rect.origin.y + rect.size.height);
	dl_CGContextClosePath(context);
}
void doClippedCircle(dl_CGContextRef context) {
	dl_CGPoint circleCenter = { 100, 100 };
	float circleRadius = 100.0;
	float startingAngle = 0.0;
	auto endingAngle = (float)(M_PI * 2.0);
	dl_CGRect ourRect = { {15, 15}, {170, 170} };
	dl_CGRect totalArea = { {0, 0}, {170 + 100 + 5 + 150 + 100 - 50, 200} };

	// white background
	dl_CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	dl_CGContextFillRect(context, totalArea);

	// filled circle
	dl_CGContextSetRGBFillColor(context, 0.663, 0, 0.031, 1);
	dl_CGContextBeginPath(context);
	// construct cirle path counterclockwise
	dl_CGContextAddArc(context, circleCenter.x, circleCenter.y, circleRadius, startingAngle, endingAngle, 0);
	dl_CGContextDrawPath(context, dl_kCGPathFill);

	// stroked square
	dl_CGContextStrokeRect(context, ourRect);

	// translate to side
	dl_CGContextTranslateCTM(context, ourRect.size.width + circleRadius + 5, 0);

	// create rect path + clip
	dl_CGContextBeginPath(context);
	dl_CGContextAddRect(context, ourRect);
	dl_CGContextClip(context);

	// circle again (clipped now)
	dl_CGContextBeginPath(context);
	// construct cirle path counterclockwise
	dl_CGContextAddArc(context, circleCenter.x, circleCenter.y, circleRadius, startingAngle, endingAngle, 0);
	dl_CGContextDrawPath(context, dl_kCGPathFillStroke);
}

class CPage01 : public PageCommon {
	int minWidth, minHeight;
	int maxWidth, maxHeight;
	int initWidth, initHeight;
	int mouse_x = -1;
	int mouse_y = -1;

	bool doCrossMask = false;

	dl_CGFloat animAngle = (M_PI * 2 * 30) / 360;
	dl_CGFloat animTurnsPerSec = 1.0 / 5.0;
	bool animating = false;

public:
	CPage01(WindowFuncs *parent, int minWidth, int minHeight, int maxWidth, int maxHeight, int initWidth, int initHeight)
	:PageCommon(parent),
		minWidth(minWidth), minHeight(minHeight), maxWidth(maxWidth), maxHeight(maxHeight),
		initWidth(initWidth), initHeight(initHeight)
	{
		//
	}

	bool isAnimating() override { return animating; }

	void onTimerEvent(double secondsSinceLast) override {
		if (animating) {
			dl_CGFloat turn_rads = animTurnsPerSec * secondsSinceLast * M_PI * 2.0;
			animAngle = fmod(animAngle + turn_rads, M_PI * 2.0);
			parent->invalidate(0, 0, 0, 0);
		}
	}

	void onMouseEvent(wl_MouseEvent &mouseEvent) override {
		if (mouseEvent.eventType == wl_kMouseEventTypeMouseDown) {
			animating = !animating;
		}
		else if (mouseEvent.eventType == wl_kMouseEventTypeMouseMove) {
			mouse_x = mouseEvent.x;
			mouse_y = mouseEvent.y;
			if (!animating) {
				parent->invalidate(0, 0, 0, 0);
			}
		}
	}

	void onKeyEvent(wl_KeyEvent &keyEvent) override {
		switch (keyEvent.key) {
			case wl_kKeyC:
				if (keyEvent.eventType == wl_kKeyEventTypeDown) {
					doCrossMask = !doCrossMask;
					if (!animating) {
						parent->invalidate(0, 0, 0, 0);
					}
				}
				break;
            default:
                break;
		}
	}

	void render(dl_CGContextRef context, int width, int height) override {
		//printf("x,y,w,h: %d,%d,%d,%d\n", event->repaintEvent.x, event->repaintEvent.y, event->repaintEvent.width, event->repaintEvent.height);

		// max-size background
		dl_CGContextSetRGBFillColor(context, 0, 0, 0, 1);
		dl_CGRect rmax = { 0, 0, (float)maxWidth, (float)maxHeight };
		dl_CGContextFillRect(context, rmax);

		// normal size background
		dl_CGContextSetRGBFillColor(context, 0.1, 0.1, 0.3, 1.0);
		dl_CGRect rback = { 0, 0, (dl_CGFloat)initWidth, (dl_CGFloat)initHeight };
		dl_CGContextFillRect(context, rback);

		if (doCrossMask) {
			// circular clip
			dl_CGRect clippy = { { mouse_x - (400.0 / 2.0), mouse_y - (400.0 / 2.0) },{ 400, 400 } };
			dl_CGContextBeginPath(context);
			auto clx = clippy.origin.x + (clippy.size.width / 2);
			auto cly = clippy.origin.y + (clippy.size.height / 2);
			dl_CGContextAddArc(context, clx, cly, clippy.size.width / 2, 0, M_PI * 2, 0);
			dl_CGContextClip(context);
			// cross inside that
			auto crossWidth = clippy.size.width / 3;
			dl_CGRect vert = { { clx - (crossWidth / 2), clippy.origin.y },{ crossWidth, clippy.size.height } };
			//        dl_CGRect vert2 = { { (clippy.origin.x + clippy.size.width) - (crossWidth / 2), clippy.origin.y },{ crossWidth, clippy.size.height } };
			dl_CGRect horz = { { clippy.origin.x, cly - (crossWidth / 2) },{ clippy.size.width, crossWidth } };
			dl_CGContextBeginPath(context);
			dl_CGContextAddRect(context, vert);
			dl_CGContextAddRect(context, horz);
			dl_CGContextClip(context);
		}

		// dashed lines
		dl_CGContextSaveGState(context);

		dl_CGPoint start = { 40, 280 };
		dl_CGPoint end = { 900, 280 };
		auto linesWidth = end.x - start.x;
		auto linesHeight = 50 * 5;
		dl_CGFloat lengths[6] = { 12, 6, 5, 6, 5, 6 };

		//dl_CGContextClipToRect(context, { { 40, 280 },{ 500, 50 * 5 } });

		// black circle behind lines
		dl_CGContextSetRGBFillColor(context, 0, 0, 0, 1);
		dl_CGContextSetRGBStrokeColor(context, 0.4, 0, 0, 1);
		dl_CGContextSetLineWidth(context, 5.0);
		dl_CGContextBeginPath(context);
		dl_CGContextAddArc(context, start.x + linesWidth / 2, start.y + linesHeight / 2, 300, 0, M_PI * 2.0, 0);
		dl_CGContextDrawPath(context, dl_kCGPathFillStroke);

		dl_CGContextBeginPath(context);
		dl_CGContextAddArc(context, start.x + linesWidth / 2, start.y + linesHeight / 2, 300, 0, M_PI * 2, 0);
		dl_CGContextClip(context);

		// DASHED LINES ===================
		dl_CGContextSetRGBStrokeColor(context, 1, 1, 0, 1);

		// line 1 solid
		dl_CGContextSetLineWidth(context, 5.0);
		drawStrokedLine(context, start, end);

		// line 2 long dashes
		dl_CGContextTranslateCTM(context, 0, 50);
		dl_CGContextSetLineDash(context, 0, lengths, 2);
		drawStrokedLine(context, start, end);

		// line 3 long short pattern
		dl_CGContextTranslateCTM(context, 0, 50);
		dl_CGContextSetLineDash(context, 0, lengths, 4);
		drawStrokedLine(context, start, end);

		// line 3.5! interrupting the show ======
		dl_CGContextSaveGState(context);

		dl_CGContextTranslateCTM(context, 0, 25);

		dl_CGContextSetLineWidth(context, 12);
		dl_CGContextSetLineDash(context, 0, nullptr, 0); // temporarily disable dashes ...
		dl_CGContextSetRGBStrokeColor(context, 0, 1, 1, 1);
		drawStrokedLine(context, start, end);

		dl_CGContextRestoreGState(context);
		// end line 3.5 ============

		// line 4 long short short
		dl_CGContextTranslateCTM(context, 0, 50);
		dl_CGContextSetLineDash(context, 0, lengths, 6);
		drawStrokedLine(context, start, end);

		// line 5 short short long
		dl_CGContextTranslateCTM(context, 0, 50);
		dl_CGContextSetLineDash(context, lengths[0] + lengths[1], lengths, 6);
		drawStrokedLine(context, start, end);

		// line 6 solid line
		dl_CGContextTranslateCTM(context, 0, 50);
		dl_CGContextSetLineDash(context, 0, NULL, 0); // reset to solid
		drawStrokedLine(context, start, end);

		dl_CGContextRestoreGState(context);
		// END DASHED LINES ==================

		// save pre-transforms
		dl_CGContextSaveGState(context);

		dl_CGContextSetRGBStrokeColor(context, 0, 1.0, 0, 1.0);
		dl_CGRect r2 = { (dl_CGFloat)100, (dl_CGFloat)100, (dl_CGFloat)250, (dl_CGFloat)100 };
		dl_CGContextStrokeRectWithWidth(context, r2, 3.0);

		dl_CGContextSetRGBStrokeColor(context, 0, 0.5, 1.0, 1.0);
		for (int i = 0; i < 5; i++) {
			dl_CGFloat n = 0.5 + ((i + 1) * 5);
			dl_CGRect r3 = { n, n, initWidth - (n * 2), initHeight - (n * 2) };
			dl_CGContextStrokeRectWithWidth(context, r3, 1.0);
		}

		dl_CGContextSetRGBStrokeColor(context, 1.0, 0, 0, 1.0);
		for (int i = 0; i < 5; i++) {
			dl_CGFloat n = 0.5 + ((i + 1) * 5);
			dl_CGRect r3 = { n, n, minWidth - (n * 2), minHeight - (n * 2) };
			dl_CGContextStrokeRectWithWidth(context, r3, 1.0);
		}

		dl_CGContextSetRGBStrokeColor(context, 0, 1.0, 0.0, 1.0);
		for (int i = 0; i < 5; i++) {
			dl_CGFloat n = 0.5 + ((i + 1) * 5);
			dl_CGRect r3 = { n, n, maxWidth - (n * 2), maxHeight - (n * 2) };
			dl_CGContextStrokeRectWithWidth(context, r3, 1.0);
		}

		dl_CGRect r4 = { 280, 100, 200.0, 200.0 };
		dl_CGFloat alpha = 1.0 / 5;
		for (int i = 0; i < 5; i++) {
			r4.origin.x += 20;
			r4.origin.y += 20;
			doBlueRect(context, r4, alpha);
			alpha += 1.0 / 5;
		}

		dl_CGRect r5 = { 0.5, 0.5, 149, 149 };
		dl_CGContextTranslateCTM(context, 280, 300);
		dl_CGContextSetRGBFillColor(context, 1, 0, .3, .2);
		dl_CGContextSetRGBStrokeColor(context, 0, 0, 0, 1);
		dl_CGContextSetLineWidth(context, 1);
		for (int i = 0; i < 5; i++) {
			createRectPath(context, r5);
			dl_CGContextDrawPath(context, (i % 2) ? dl_kCGPathFill : dl_kCGPathFillStroke);
			dl_CGContextTranslateCTM(context, -15.0, 15.0);
		}

		dl_CGContextRestoreGState(context);

		// alpha rects ==========================================
		dl_CGContextSaveGState(context);

		dl_CGRect r6 = { 0, 0, 130, 100 };
		dl_CGRect r7 = { 120, 90, 5, 5 };
		int numRects = 6;
		dl_CGFloat tint = 1, tintAdjust = 1.0 / numRects;
		dl_CGFloat rotAngle = (M_PI * 2) / numRects;

		dl_CGContextTranslateCTM(context, 800, 400);
		dl_CGContextScaleCTM(context, 3.0, 3.0);

		dl_CGContextRotateCTM(context, animAngle);

		auto rounded = dl_CGPathCreateWithRoundedRect(r6, 10, 10, nullptr);
        auto cornerCircle = dl_CGPathCreateWithEllipseInRect(r7, nullptr);
		for (int i = 0; i < numRects; i++) {
			dl_CGContextSetRGBFillColor(context, tint, tint / 2, 0, tint);
			dl_CGContextAddPath(context, rounded);
			dl_CGContextFillPath(context);

			dl_CGContextSetRGBStrokeColor(context, 0, 0, 0, 1.0);
			dl_CGContextAddPath(context, rounded);
            dl_CGContextSetLineWidth(context, 1.5);
			dl_CGContextStrokePath(context);
            
            // yellow corner thing
			dl_CGContextSetRGBFillColor(context, 1, 1, 0, 1);
            dl_CGContextAddPath(context, cornerCircle);
            dl_CGContextFillPath(context);

			dl_CGContextRotateCTM(context, rotAngle);
			tint -= tintAdjust;
		}
        dl_CGPathRelease(cornerCircle);
		dl_CGPathRelease(rounded);

		dl_CGContextRestoreGState(context);


		// clipping example =====================================
		dl_CGContextSaveGState(context);

		dl_CGContextScaleCTM(context, 0.75, 0.75);
		dl_CGContextTranslateCTM(context, 70, 740);

		doClippedCircle(context);

		dl_CGContextRestoreGState(context);
        
        // curved corner triangle thing =========================
        dl_CGContextSaveGState(context);
        
        dl_CGPoint points[] = { {150, 150}, {540, 340}, {130, 540}};
        
        // line version
        dl_CGContextMoveToPoint(context, points[0].x, points[0].y);
        dl_CGContextAddLineToPoint(context, points[1].x, points[1].y);
        dl_CGContextAddLineToPoint(context, points[2].x, points[2].y);
        dl_CGContextClosePath(context);
        //
        dl_CGContextSetRGBStrokeColor(context, 1, 1, 1, 0.5);
        dl_CGContextSetLineWidth(context, 1.0);
        dl_CGContextStrokePath(context);
        
        // curved version
        dl_CGPoint startEnd = betweenPoints(points[2], points[0]);
        dl_CGContextMoveToPoint(context, startEnd.x, startEnd.y);
        dl_CGContextAddArcToPoint(context, points[0].x, points[0].y, points[1].x, points[1].y, 20.0);
        dl_CGContextAddArcToPoint(context, points[1].x, points[1].y, points[2].x, points[2].y, 20.0);
        dl_CGContextAddArcToPoint(context, points[2].x, points[2].y, points[0].x, points[0].y, 20.0);
        dl_CGContextClosePath(context);
        //
        dl_CGContextSetRGBStrokeColor(context, 1, 1, 0, 1);
        dl_CGContextSetLineWidth(context, 2.0);
        dl_CGContextStrokePath(context);
        
        dl_CGContextRestoreGState(context);
	}
};

#endif
