#include "CGContext.h"

#include "../../COMStuff.h"
#include "../../../common/geometry.h"
#include "../../directwrite/CoreTextFormatSpec.h"

//inline void geomSinkAddRect(ID2D1GeometrySink *geomSink, PathElement &e, D2D1_FIGURE_BEGIN fillType, bool *figureOpen)
//{
//	auto point = D2D1::Point2F((FLOAT)e.rect.origin.x, (FLOAT)e.rect.origin.y);
//	geomSink->BeginFigure(point, fillType);
//	*figureOpen = true;
//
//	point.x += (FLOAT)e.rect.size.width;
//	geomSink->AddLine(point);
//
//	point.y += (FLOAT)e.rect.size.height;
//	geomSink->AddLine(point);
//
//	point.x -= (FLOAT)e.rect.size.width;
//	geomSink->AddLine(point);
//
//	//point.y -= (FLOAT)e.rect.size.height;
//	//geomSink->AddLine(point); // implicitly closed, I guess?
//	geomSink->EndFigure(D2D1_FIGURE_END_CLOSED);
//	*figureOpen = false;
//}
//
//inline void geomSinkAddArc(ID2D1GeometrySink *geomSink, PathElement &e, D2D1_FIGURE_BEGIN fillType, bool *figureOpen)
//{
//	dl_CGFloat clockMul = e.arc.clockwise ? 1.0 : -1.0;
//	dl_CGFloat p1x = e.arc.x + (cos(clockMul * e.arc.startAngle) * e.arc.radius);
//	dl_CGFloat p1y = e.arc.y + (sin(clockMul * e.arc.startAngle) * e.arc.radius);
//	auto p1 = D2D1::Point2F((FLOAT)p1x, (FLOAT)p1y);
//	geomSink->BeginFigure(p1, fillType);
//	*figureOpen = true;
//
//	// this would probably be better to do with epsilon / c++ numeric limits stuff
//	// basically make sure they're not equal (after an fmod(M_PI*2)), otherwise windows puts 360-degree circles in the wrong place
//	auto diff = fmod(abs(e.arc.endAngle - e.arc.startAngle), M_PI * 2.0);
//	bool fullCircle = diff < 0.0001;
//	if (fullCircle) {
//		e.arc.endAngle *= 0.9999;
//		// and we'll have to manually EndFigure because otherwise there'd be a gap in the stroke
//	}
//
//	dl_CGFloat p2x = e.arc.x + (cos(clockMul * e.arc.endAngle) * e.arc.radius);
//	dl_CGFloat p2y = e.arc.y + (sin(clockMul * e.arc.endAngle) * e.arc.radius);
//	auto p2 = D2D1::Point2F((FLOAT)p2x, (FLOAT)p2y);
//
//	auto size = D2D1::SizeF((FLOAT)e.arc.radius, (FLOAT)e.arc.radius);
//	auto dir = e.arc.clockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
//	auto angle = ((e.arc.endAngle - e.arc.startAngle) * 360.0) / (M_PI * 2.0);
//	auto big = (angle >= 180.0) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
//	auto arc = D2D1::ArcSegment(p2, size, 0, dir, big);
//	geomSink->AddArc(arc);
//
//	if (fullCircle) {
//		geomSink->EndFigure(D2D1_FIGURE_END_CLOSED);
//		*figureOpen = false;
//	}
//}
//
//inline void geomSinkAddEllipse(ID2D1GeometrySink *geomSink, PathElement &e, D2D1_FIGURE_BEGIN fillType, bool *figureOpen)
//{
//	ID2D1EllipseGeometry *geom;
//	HR(d2dFactory->CreateEllipseGeometry(&e.ellipse, &geom));
//
//	// begin figure? fill type? etc?
//	HR(geom->Outline(NULL, geomSink));
//
//	//// really not sure ...
//	//geomSink->EndFigure(D2D1_FIGURE_END_CLOSED);
//	//*figureOpen = false;
//
//	SafeRelease(&geom);
//}
//
//inline void geomSinkAddRoundedRect(ID2D1GeometrySink *geomSink, PathElement &e, D2D1_FIGURE_BEGIN fillType, bool *figureOpen)
//{
//	ID2D1RoundedRectangleGeometry *geom;
//	HR(d2dFactory->CreateRoundedRectangleGeometry(&e.roundedRect, &geom));
//
//	// begin figure? fill type? etc?
//	HR(geom->Outline(NULL, geomSink));
//
//	// not sure
//	// geomSink->EndFigure...
//	// *figureOpen = false
//
//	SafeRelease(&geom);
//}
//
//inline void geomSinkAddArcToPoint(ID2D1GeometrySink *geomSink, PathElement &e, D2D1_POINT_2F *lastPoint)
//{
//	double centerX, centerY, startX, startY, angle0, angle1;
//
//	calcArcToPoint(
//		lastPoint->x, lastPoint->y,
//		e.arcToPoint.x1, e.arcToPoint.y1,
//		e.arcToPoint.x2, e.arcToPoint.y2,
//		e.arcToPoint.radius,
//		&centerX, &centerY, &startX, &startY, &angle0, &angle1);
//
//	// draw the damn thing
//	geomSink->AddLine(D2D1::Point2F((FLOAT)startX, (FLOAT)startY));
//	auto endX = centerX + cos(angle1) * e.arcToPoint.radius;
//	auto endY = centerY + sin(angle1) * e.arcToPoint.radius;
//	auto endPoint = D2D1::Point2F((FLOAT)endX, (FLOAT)endY);
//	auto arc = D2D1::ArcSegment(
//		endPoint,
//		D2D1::SizeF((FLOAT)e.arcToPoint.radius, (FLOAT)e.arcToPoint.radius),
//		0,
//		D2D1_SWEEP_DIRECTION_CLOCKWISE,
//		(angle1 - angle0) > M_PI ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL);
//	geomSink->AddArc(arc);
//	// we end at the end of that arc, resume drawing from there
//	*lastPoint = endPoint;
//}

//ID2D1PathGeometry *CGContext::pathFromElements(D2D1_FIGURE_BEGIN fillType)
//{
//	ID2D1PathGeometry *currentPath = nullptr;
//	ID2D1GeometrySink *geomSink = nullptr;
//	d2dFactory->CreatePathGeometry(&currentPath);
//	currentPath->Open(&geomSink);
//	geomSink->SetFillMode(D2D1_FILL_MODE_WINDING);
//
//	bool figureOpen = false;
//	D2D1_POINT_2F figureBeginPoint, lastPoint;
//
//	for (auto item = pathElements.begin(); item < pathElements.end(); item++) {
//		switch (item->elementType) {
//		case PathElement_StartPoint:
//			geomSink->BeginFigure(D2D1::Point2F((FLOAT)item->point.x, (FLOAT)item->point.y), fillType);
//			figureOpen = true;
//			figureBeginPoint = D2D1::Point2F((FLOAT)item->point.x, (FLOAT)item->point.y);
//			lastPoint = figureBeginPoint;
//			break;
//		case PathElement_LineToPoint:
//			geomSink->AddLine(D2D1::Point2F((FLOAT)item->point.x, (FLOAT)item->point.y));
//			lastPoint = D2D1::Point2F((FLOAT)item->point.x, (FLOAT)item->point.y);
//			break;
//		case PathElement_Rect:
//			// TODO: look at this and arc and see if they can be simplified, a la ellipse and rounded rect (using D2D geometries directly writing to sink)
//			geomSinkAddRect(geomSink, *item, fillType, &figureOpen);
//			lastPoint = D2D1::Point2F((FLOAT)item->rect.origin.x, (FLOAT)item->rect.origin.y);
//			break;
//		case PathElement_Arc: {
//			// TODO: see todo above
//			geomSinkAddArc(geomSink, *item, fillType, &figureOpen); // might be a full circle, so pass didClose to be modified in there
//			auto endX = item->arc.x + cos(item->arc.endAngle) * item->arc.radius;
//			auto endY = item->arc.y + sin(item->arc.endAngle) * item->arc.radius;
//			lastPoint = D2D1::Point2F((FLOAT)endX, (FLOAT)endY);
//			break;
//		}
//		case PathElement_ArcToPoint:
//			geomSinkAddArcToPoint(geomSink, *item, &lastPoint); // updates lastPoint
//			break;
//		case PathElement_Ellipse: {
//			geomSinkAddEllipse(geomSink, *item, fillType, &figureOpen);
//			auto endX = item->ellipse.point.x + item->ellipse.radiusX;
//			auto endY = item->ellipse.point.y;
//			lastPoint = D2D1::Point2F((FLOAT)endX, (FLOAT)endY);
//			break;
//		}
//		case PathElement_RoundedRect:
//			geomSinkAddRoundedRect(geomSink, *item, fillType, &figureOpen);
//			// TODO: not sure what lastPoint should be here ...
//			break;
//		case PathElement_Closure:
//			geomSink->EndFigure(D2D1_FIGURE_END_CLOSED);
//			figureOpen = false;
//			lastPoint = figureBeginPoint;
//			break;
//		}
//	}
//	if (figureOpen) {
//		geomSink->EndFigure(D2D1_FIGURE_END_OPEN);
//	}
//	geomSink->Close();
//	SafeRelease(&geomSink);
//
//	return currentPath;
//}

void CGContext::clipTargetByHalfPlane(dl_CGPoint hp_point, dl_CGPoint hp_vec) {
	auto size = target->GetSize();
	dl_CGRect rect = { {0, 0}, {size.width, size.height} };

	// inverse transform viewport rect, into current transform space
	D2D1::Matrix3x2F m;
	target->GetTransform(&m);
	m.Invert();
	D2D1_POINT_2F rawCorners[4] = {
		m.TransformPoint(D2D1::Point2F(0, 0)),
		m.TransformPoint(D2D1::Point2F(size.width, 0)),
		m.TransformPoint(D2D1::Point2F(size.width, size.height)),
		m.TransformPoint(D2D1::Point2F(0, size.height))
	};
	dl_CGPoint viewCorners[4] = {
		dl_CGPointMake(rawCorners[0].x, rawCorners[0].y),
		dl_CGPointMake(rawCorners[1].x, rawCorners[1].y),
		dl_CGPointMake(rawCorners[2].x, rawCorners[2].y),
		dl_CGPointMake(rawCorners[3].x, rawCorners[3].y),
	};
	//if (pointIsCoincident(hp_point, viewCorners, 4)) {
	//	// skip this point, it would clip nothing
	//	// EDIT: wait, what? makes no sense ...
	//	printf("### CGContext::clipTargetByHalfPlane - pointIsCoincident() was true - pretty sure this shouldn't even be here ...\n");
	//}
	//else {
		dl_CGPoint clipPoints[10]; // pretty sure the mathematical max is 5, but just in case ...
		int clipPointCount = 0;
		//clipRectByHalfPlane(rect, hp_point, hp_vec, clipPoints, &clipPointCount);
		clipPolyByHalfPlane(viewCorners, 4, hp_point, hp_vec, clipPoints, &clipPointCount);

		//dl_CGContextBeginPath(c);
		beginPath();

		moveToPoint(clipPoints[0].x, clipPoints[0].y);
		for (int i = 1; i < clipPointCount; i++) {
			addLineToPoint(clipPoints[i].x, clipPoints[i].y);
		}

		//dl_CGContextClosePath(c);
		//dl_CGContextClip(c);
		closePath();
		clipCurrentPath();
	//}
}

void CGContext::fillStrokeClip(
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_GLYPH_RUN const* glyphRun,
	ID2D1SolidColorBrush *fillBrush,  // note: fill/stroke param order reversed from old CoreTextRenderer!
	ID2D1SolidColorBrush *strokeBrush,
	FLOAT strokeWidth,
	bool doClip // default: false
	) 
{
	auto pTransformedGeometry = getGlyphRunGeometry(
		baselineOriginX,
		baselineOriginY,
		glyphRun
	);

	if (fillBrush) {
		// Fill in the glyph run
		target->FillGeometry(
			pTransformedGeometry,
			fillBrush
		);
	}

	if (strokeBrush) {
		// Draw the outline of the glyph run
		target->DrawGeometry(
			pTransformedGeometry,
			strokeBrush,
			strokeWidth
		);
	}

	if (doClip) {
		//// need to ACCUMULATE the clip, because multiple glyph runs may be part of the same text draw call, and they all have to be included
		//// (clipping intersects the existing clip, so multiple clips will not give the proper result)
		//accumulateTextClip(pTransformedGeometry);
		clipToPath(pTransformedGeometry);
	}

	SafeRelease(&pTransformedGeometry);
}

void CGContext::commonTextDraw(
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_GLYPH_RUN const* glyphRun,
	ID2D1SolidColorBrush *fillBrush,  // note: fill/stroke param order reversed from old CoreTextRenderer!
	ID2D1SolidColorBrush *strokeBrush,
	FLOAT strokeWidth
	)
{
	switch (textDrawingMode) {
	case dl_kCGTextFill:
	case dl_kCGTextFillClip:
		target->DrawGlyphRun(
			D2D1::Point2F(baselineOriginX, baselineOriginY),
			glyphRun,
			fillBrush
		);
		break;
	case dl_kCGTextStroke:
	case dl_kCGTextStrokeClip:
		fillStrokeClip(
			baselineOriginX,
			baselineOriginY,
			glyphRun,
			nullptr, // no fill, stroke only
			strokeBrush,
			strokeWidth,
			textDrawingMode == dl_kCGTextStrokeClip
		);
		break;
	case dl_kCGTextFillStroke:
	case dl_kCGTextFillStrokeClip:
		fillStrokeClip(
			baselineOriginX,
			baselineOriginY,
			glyphRun,
			fillBrush,
			strokeBrush,
			strokeWidth,
			textDrawingMode == dl_kCGTextFillStrokeClip
		);
		break;
	case dl_kCGTextClip:
		fillStrokeClip(
			baselineOriginX,
			baselineOriginY,
			glyphRun,
			nullptr,
			nullptr,
			0.0,
			true
		);
		break;
	case dl_kCGTextInvisible:
		// just advance the text drawing point
		break;
	default:
		printf("unknown text drawing mode %d\n", textDrawingMode);
	}
}

template <typename T>
inline T ELVIS(T x, T y) { // elvis operator in other languages
	if (x) {
		return x;
	}
	else {
		return y;
	}
}

void CGContext::drawGlyphRun(
	void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode,
	DWRITE_GLYPH_RUN const* glyphRun,
	DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
	IUnknown* clientDrawingEffect,
	// some extra (non IDWriteTextRenderer) params below
	// go to CoreTextRenderer (which calls this) if you need more
	DWRITE_LINE_METRICS *metrics,
	ID2D1SolidColorBrush* pDefaultBrush
	)
{
	//auto ctfs = dynamic_cast<CoreTextFormatSpec *>(clientDrawingEffect);
	auto ctfs = (CoreTextFormatSpec *)clientDrawingEffect;
	if (ctfs) {
		float strokeWidth;
		bool hasStroke = ctfs->getStrokeWidth(&strokeWidth);

		// foreground fallback logic:
		// use specific foreground brush color if specified,
		// otherwise use the context foreground if it's specifically asked for,
		// otherwise use the default brush (probably black)
		auto foreBrush =
			ELVIS(ctfs->getForegroundBrush(target),
			(ctfs->useContextForegroundColor() ? drawState()->fillBrush : pDefaultBrush));

		if (hasStroke) {
			// stroke attribute present, override textDrawingMode (and choose to fill or not based on stroke width sign)
			auto strokeBrush = ELVIS(ctfs->getStrokeColor(target), pDefaultBrush);
			strokeWidth /= (float)getTextScaleRatio();

			if (strokeWidth < 0) {
				// if negative stroke width, fill the text with the foreground (or default) color and stroke with the stroke color (if any)
				fillStrokeClip(
					baselineOriginX,
					baselineOriginY,
					glyphRun,
					foreBrush,
					strokeBrush,
					(FLOAT)(strokeWidth * -1.0), // convert from negative
					false // clipping always ignored when stroke specified
				);
			}
			else {
				// if positive stroke width, stroke the text with foreground (or default) color
				fillStrokeClip(
					baselineOriginX,
					baselineOriginY,
					glyphRun,
					nullptr, // stroke only
					strokeBrush,
					strokeWidth,
					false // clipping always ignored when stroke specified
				);
			}
		}
		else {
			// stroke wasn't specified, use textDrawingMode (and context line width)
			auto strokeBrush = ELVIS(ctfs->getStrokeColor(target), ELVIS(drawState()->strokeBrush, pDefaultBrush));
			strokeWidth = (float)(drawState()->lineWidth / getTextScaleRatio());

			commonTextDraw(
				baselineOriginX,
				baselineOriginY,
				glyphRun,
				foreBrush,
				strokeBrush,
				strokeWidth
			);
		}
	}
	else {
		// vanilla rendering based on text mode (glyph range had no dict/attributes)
		// but then how would font/size/etc be defined??
		commonTextDraw(
			baselineOriginX,
			baselineOriginY,
			glyphRun,
			drawState()->fillBrush, // or pDefaultBrush?
			drawState()->strokeBrush,
			(FLOAT)(drawState()->lineWidth / getTextScaleRatio())
		);
	}
}
