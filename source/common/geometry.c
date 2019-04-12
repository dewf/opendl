#include "geometry.h"

#include <assert.h>
#include <math.h>

#define GHETTO_EPSILON 0.0001  // from what I understand a LOT more thought has to go into this, and it should be relative to the magnitude of the numbers being used, etc etc


dl_CGPoint normalizeVec2F(dl_CGPoint vec2f)
{
	double len = sqrt((vec2f.x * vec2f.x) + (vec2f.y * vec2f.y));
	if (len > 0) {
		return (dl_CGPoint){ vec2f.x / len, vec2f.y / len };
	} else {
		return dl_CGPointZero; // is this correct, for pointInHalfPlane below? or should we avoid even doing the dot calculation in this case?
	}
}

bool pointInHalfPlane(dl_CGPoint point, dl_CGPoint hp_point, dl_CGPoint hp_vec)
{
	dl_CGPoint vec2 = normalizeVec2F((dl_CGPoint){ point.x - hp_point.x, point.y - hp_point.y });
	dl_CGFloat dot = (vec2.x * hp_vec.x) + (vec2.y * hp_vec.y);
	return dot >= -GHETTO_EPSILON;
}

IntersectResult segmentIntersectLine(dl_CGPoint p1, dl_CGPoint p2, dl_CGPoint line_point, dl_CGPoint line_vec)
{
	IntersectResult res;

	dl_CGPoint p3 = line_point;
	dl_CGPoint p4 = { line_point.x + line_vec.x, line_point.y + line_vec.y };
	dl_CGFloat u_num = ((p4.x - p3.x) * (p1.y - p3.y)) - ((p4.y - p3.y) * (p1.x - p3.x));
	dl_CGFloat u_denom = ((p4.y - p3.y) * (p2.x - p1.x)) - ((p4.x - p3.x) * (p2.y - p1.y));
	if (u_denom == 0) {
		if (u_num == 0) {
			res.resultType = Coincident;
		}
		else {
			res.resultType = Parallel;
		}
	}
	else {
		dl_CGFloat ua = u_num / u_denom;
		if (fabs(ua) <= GHETTO_EPSILON) ua = 0.0;
		if (fabs(ua - 1.0) <= GHETTO_EPSILON) ua = 1.0;
		if (ua >= 0.0 && ua <= 1.0) {
			dl_CGFloat x = p1.x + ua * (p2.x - p1.x);
			dl_CGFloat y = p1.y + ua * (p2.y - p1.y);
			res.resultType = PointResult;
			res.point = (dl_CGPoint){ x, y };
		}
		else {
			res.resultType = OutOfSegment;
		}
	}
	return res;
}

void cornersFromRect(dl_CGRect rect, dl_CGPoint corners[4]) {
	corners[0] = (dl_CGPoint) { rect.origin.x, rect.origin.y };
	corners[1] = (dl_CGPoint) { rect.origin.x + rect.size.width, rect.origin.y };
	corners[2] = (dl_CGPoint) { rect.origin.x + rect.size.width, rect.origin.y + rect.size.height };
	corners[3] = (dl_CGPoint) { rect.origin.x, rect.origin.y + rect.size.height };
}

bool pointIsCorner(dl_CGRect rect, dl_CGPoint p)
{
	dl_CGPoint corners[4];
	cornersFromRect(rect, corners);
	for (int i = 0; i < 4; i++) {
		if (fabs(p.x - corners[i].x) <= GHETTO_EPSILON &&
			fabs(p.y - corners[i].y) <= GHETTO_EPSILON)
		{
			return true;
		}
	}
	return false;
}

void clipRectByHalfPlane(dl_CGRect rect, dl_CGPoint hp_point, dl_CGPoint hp_vec, dl_CGPoint *outPoints, int *outCount)
{
	// get line perpendicular to vec2f through point (rotating left)
	dl_CGPoint perp_vec = normalizeVec2F((dl_CGPoint){ hp_vec.y, -hp_vec.x });

	dl_CGPoint corners[4];
	cornersFromRect(rect, corners);
	//dl_CGPoint corners[] = {
	//	{ rect.origin.x, rect.origin.y },
	//	{ rect.origin.x + rect.size.width, rect.origin.y },
	//	{ rect.origin.x + rect.size.width, rect.origin.y + rect.size.height },
	//	{ rect.origin.x, rect.origin.y + rect.size.height }
	//};

	for (int i = 0; i < 4; i++) {
		// segment to intersect
		dl_CGPoint p1 = corners[i];
		dl_CGPoint p2 = corners[(i + 1) % 4];

		// check points of segment
		// technically we're checking all points twice, wasteful but no need to optimize that away right now
		bool p1_in = pointInHalfPlane(p1, hp_point, hp_vec);
		bool p2_in = pointInHalfPlane(p2, hp_point, hp_vec);

		// if both points are on correct side, add whole segment
		// if neither points is, skip segment
		// if just one point, add that point and line intersection point (in correct order)
		// (keeping in mind that we only ever add the starting point of a segment, the next loop iteration will take care of the former iteration endpoint)
		if (p1_in && p2_in) {
			// add both points to shape
			outPoints[(*outCount)++] = p1;
			// p2 will be added next iter
		}
		else if (p1_in && !p2_in) {
			// add p1, then the intersected point
			outPoints[(*outCount)++] = p1;
			IntersectResult newP = segmentIntersectLine(p1, p2, hp_point, perp_vec);
			assert(newP.resultType == PointResult);
			outPoints[(*outCount)++] = newP.point;
		}
		else if (!p1_in && p2_in) {
			// add intersected point, then p2 (on next iter)
			IntersectResult newP = segmentIntersectLine(p1, p2, hp_point, perp_vec);
			assert(newP.resultType == PointResult);
			outPoints[(*outCount)++] = newP.point;
			// p2 will be added on next iter
		}
		else if (!p1_in && !p2_in) {
			// add neither
		}
	}
}
