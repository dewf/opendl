#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "../opendl.h"

#ifdef __cplusplus
extern "C" {
#endif

	dl_CGPoint normalizeVec2F(dl_CGPoint vec2f);

	dl_CGPoint applyTransform(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y);

	bool pointInHalfPlane(dl_CGPoint point, dl_CGPoint hp_point, dl_CGPoint hp_vec);

	bool pointIsCoincident(dl_CGPoint p, dl_CGPoint *compare, int compareCount);
	bool pointIsCorner(dl_CGRect rect, dl_CGPoint p);

	typedef enum {
		PointResult,
		OutOfSegment,
		Parallel,
		Coincident
	} IntersectResultType;

	typedef struct {
		IntersectResultType resultType;
		dl_CGPoint point;
	} IntersectResult;

	IntersectResult segmentIntersectLine(dl_CGPoint p1, dl_CGPoint p2, dl_CGPoint line_point, dl_CGPoint line_vec);
	void clipPolyByHalfPlane(dl_CGPoint *points, int numPoints, dl_CGPoint hp_point, dl_CGPoint hp_vec, dl_CGPoint *outPoints, int *outCount);
	//void clipRectByHalfPlane(dl_CGRect rect, dl_CGPoint hp_point, dl_CGPoint hp_vec, dl_CGPoint *outPoints, int *outCount);

	inline double vectorLength(double dx, double dy);
	inline void normalizeVector(double &dx, double &dy);
	double angleFromPointOnCircle(double cx, double cy, double x, double y);
	void calcArcToPoint(double x0, double y0, double x1, double y1, double x2, double y2, double radius,
		double *outCenterX, double *outCenterY,
		double *outStartX, double *outStartY,
		double *outAngle0, double *outAngle1);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif // __GEOMETRY_H__
