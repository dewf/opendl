#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "../opendl.h"

#ifdef __cplusplus
extern "C" {
#endif

	dl_CGPoint normalizeVec2F(dl_CGPoint vec2f);

	bool pointInHalfPlane(dl_CGPoint point, dl_CGPoint hp_point, dl_CGPoint hp_vec);

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
	void clipRectByHalfPlane(dl_CGRect rect, dl_CGPoint hp_point, dl_CGPoint hp_vec, dl_CGPoint *outPoints, int *outCount);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif // __GEOMETRY_H__
