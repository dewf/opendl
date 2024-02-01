#ifndef __PATHSTUFF_H__
#define __PATHSTUFF_H__

#include "../opendl.h"
#include "geometry.h"

struct PathSegment {
	enum TagEnum {
		Tag_MoveToPoint,
		Tag_LineToPoint,
		Tag_Arc,
		Tag_ArcToPoint,
		Tag_CurveToPoint,
		Tag_QuadCurveToPoint,
		Tag_Closure
	} tag;
	union {
		struct {
			dl_CGFloat x, y;
		} point; // StartPoint, LineToPoint

		struct {
			dl_CGFloat x, y, radius, startAngle, endAngle;
			bool clockwise;
		} arc; // Arc

		struct {
			dl_CGFloat x1, y1, x2, y2, radius;
		} arcToPoint;

		struct {
			dl_CGFloat cp1x, cp1y, cp2x, cp2y, x, y;
		} curveToPoint;

		struct {
			dl_CGFloat cpx, cpy, x, y;
		} quadCurveToPoint;
	};
	PathSegment(TagEnum tag) {
		this->tag = tag;
	}
	static PathSegment mkMoveToPoint(dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_MoveToPoint);
		seg.point.x = x;
		seg.point.y = y;
		return seg;
	}
	static PathSegment mkLineToPoint(dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_LineToPoint);
		seg.point.x = x;
		seg.point.y = y;
		return seg;
	}
	static PathSegment mkArc(dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise) {
		PathSegment seg(Tag_Arc);
		seg.arc.x = x;
		seg.arc.y = y;
		seg.arc.radius = radius;
		seg.arc.startAngle = startAngle;
		seg.arc.endAngle = endAngle;
		seg.arc.clockwise = clockwise;
		return seg;
	}
	//static PathSegment mkRelativeArc(dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta) {
	//	PathSegment seg(Tag_RelativeArc);
	//	seg.arc.x = x;
	//	seg.arc.y = y;
	//	seg.arc.radius = radius;
	//	seg.arc.startAngle = startAngle;
	//	seg.arc.delta = delta;
	//	return seg;
	//}
	static PathSegment mkArcToPoint(dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius) {
		PathSegment seg(Tag_ArcToPoint);
		seg.arcToPoint.x1 = x1;
		seg.arcToPoint.y1 = y1;
		seg.arcToPoint.x2 = x2;
		seg.arcToPoint.y2 = y2;
		seg.arcToPoint.radius = radius;
		return seg;
	}
	static PathSegment mkCurveToPoint(dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_CurveToPoint);
		seg.curveToPoint.cp1x = cp1x;
		seg.curveToPoint.cp1y = cp1y;
		seg.curveToPoint.cp2x = cp2x;
		seg.curveToPoint.cp2y = cp2y;
		seg.curveToPoint.x = x;
		seg.curveToPoint.y = y;
		return seg;
	}
	static PathSegment mkQuadCurveToPoint(dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y) {
		PathSegment seg(Tag_QuadCurveToPoint);
		seg.quadCurveToPoint.cpx = cpx;
		seg.quadCurveToPoint.cpy = cpy;
		seg.quadCurveToPoint.x = x;
		seg.quadCurveToPoint.y = y;
		return seg;
	}
	static PathSegment mkClosure() {
		PathSegment seg(Tag_Closure);
		return seg;
	}
};

struct SubPath {
	enum SubPathTag {
		Tag_Rect,
		Tag_Ellipse,
		Tag_RoundedRect,
		Tag_Segmented
	} tag;

	dl_CGPoint startPoint = dl_CGPointZero;
	dl_CGPoint endPoint = dl_CGPointZero;

	bool hasTransform = false;
	dl_CGAffineTransform transform;		// current limitation: all items of a given subpath must share same transform
										// (whereas the quartz methods allow each segment to have its own)

	std::vector<PathSegment> segments;  // really belongs in the path struct but that causes C++ issues so ... here instead

	union {
		struct {
			dl_CGRect value;
		} rect;
		struct {
			dl_CGRect inRect;
		} ellipse;
		struct {
			dl_CGRect rect;
			dl_CGFloat cornerWidth, cornerHeight;
		} rounded;
		struct {
			int reserved;
			//std::vector<PathSegment> segments; // moved out of union because issues
		} segmented;
	};

	SubPath(SubPathTag tag) {
		this->tag = tag;
	}

	bool isRect() {
		return tag == Tag_Rect;
	}

	dl_CGRect getRect() {
		return rect.value;
	}

	void concatTransform(const dl_CGAffineTransform &t) {
		if (hasTransform) {
			transform = dl_CGAffineTransformConcat(transform, t); // verify order etc
			// do we need to set the 'check' pointer if this is a segmented path?
		}
		else {
			hasTransform = true;
			transform = t;
		}
	}

	static SubPath createRect(const dl_CGRect &r, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_Rect);
		sp.rect.value = r;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		sp.endPoint = applyTransform(t, r.origin.x, r.origin.y);
		return sp;
	}

	static SubPath createEllipse(const dl_CGRect &r, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_Ellipse);
		sp.ellipse.inRect = r;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		// TODO: not sure about the endpoint here ...
		return sp;
	}

	static SubPath createRoundedRect(const dl_CGRect &r, dl_CGFloat cornerWidth, dl_CGFloat cornerHeight, const dl_CGAffineTransform *t) {
		SubPath sp(Tag_RoundedRect);
		sp.rounded.rect = r;
		sp.rounded.cornerWidth = cornerWidth;
		sp.rounded.cornerHeight = cornerHeight;
		if (t) {
			sp.hasTransform = true;
			sp.transform = *t;
		}
		sp.endPoint = applyTransform(t, r.origin.x, r.origin.y);
		return sp;
	}

	static SubPath createEmptyPath() {
		SubPath sp(Tag_Segmented);
		// sp.segmented.check = nullptr;
		// no transform yet, that will be initialized/verified with each segment add
		return sp;
	}

	static inline bool transformsAreEqual(const dl_CGAffineTransform& a, const dl_CGAffineTransform& b) {
		return (a.a == b.a)
			&& (a.b == b.b)
			&& (a.c == b.c)
			&& (a.d == b.d)
			&& (a.tx == b.tx)
			&& (a.ty == b.ty);
	}

	void validateTransform(const dl_CGAffineTransform *m) {
		// user-provided 'm' is something:
		if (m != nullptr) {
			//   - on 1st segment, set transform to m / hasTransform = true
			if (segments.size() == 0) {
				if (!hasTransform) {
					// first element, first provided matrix - OK
					transform = *m;
					hasTransform = true;
				}
				else {
					throw cf::Exception("CGPath SubPath validation: segments.size() == 0 but already had transform? unhandled condition");
				}
			}
			//   - on 2nd+ segment, do we have an existing transform?
			else if (segments.size() > 0) {
				//     - if yes, verify m matches existing completely, else error
				if (hasTransform) {
					if (transformsAreEqual(transform, *m)) {
						// OK
					}
					else {
						throw cf::Exception("CGPath SubPath validation: provided transform didn't match existing on this subpath (present limitation win/linux)");
					}
				}
				//     - if no, immediate error because transform needed to be provided on 1st segment, but wasn't provided until now
				else {
					throw cf::Exception("CGPath SubPath validation: provided 1st subpath transform on non-1st segment (win/linux limitation)");
				}
			}
		}
		// user-provided 'm' is nothing:
		else {
			//   - verify we don't have any set transform already
			if (hasTransform) {
				// oops, the user provided something previously, but this call provided null ...
				throw cf::Exception("CGPath SubPath validation: all subpath segment transforms must match (null provided, didn't match prior transform)");
			}
		}
	}

	inline void moveToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(PathSegment::mkMoveToPoint(x, y));

		startPoint = applyTransform(m, x, y); // need to store this to go back to when/if we close
		endPoint = startPoint;
	}

	inline void addLineToPoint(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(PathSegment::mkLineToPoint(x, y));

		endPoint = applyTransform(m, x, y);
	}

	inline void addLines(const dl_CGAffineTransform *m, const dl_CGPoint *points, size_t count) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		//
		if (count < 1) return;
		segments.push_back(PathSegment::mkMoveToPoint(points->x, points->y));
		for (size_t i = 1; i < count; i++) {
			auto p = &points[i];
			segments.push_back(PathSegment::mkLineToPoint(p->x, p->y));
		}

		auto end = &points[count - 1];
		endPoint = applyTransform(m, end->x, end->y);
	}

	inline void addArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat endAngle, bool clockwise) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(PathSegment::mkArc(x, y, radius, startAngle, endAngle, clockwise));

		auto x2 = x + cos(endAngle) * radius;
		auto y2 = y + sin(endAngle) * radius;
		endPoint = applyTransform(m, x2, y2);
	}

	//inline void addRelativeArc(const dl_CGAffineTransform *m, dl_CGFloat x, dl_CGFloat y, dl_CGFloat radius, dl_CGFloat startAngle, dl_CGFloat delta) {
	//	if (tag != Tag_Segmented) return;
	//	validateTransform(m);
	//	segments.push_back(PathSegment::mkRelativeArc(x, y, radius, startAngle, delta));
	//}

	inline void addArcToPoint(const dl_CGAffineTransform *m, dl_CGFloat x1, dl_CGFloat y1, dl_CGFloat x2, dl_CGFloat y2, dl_CGFloat radius) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(PathSegment::mkArcToPoint(x1, y1, x2, y2, radius));

		endPoint = applyTransform(m, x2, y2);
	}

	inline void addCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cp1x, dl_CGFloat cp1y, dl_CGFloat cp2x, dl_CGFloat cp2y, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(PathSegment::mkCurveToPoint(cp1x, cp1y, cp2x, cp2y, x, y));

		endPoint = applyTransform(m, x, y);
	}

	inline void addQuadCurveToPoint(const dl_CGAffineTransform *m, dl_CGFloat cpx, dl_CGFloat cpy, dl_CGFloat x, dl_CGFloat y) {
		if (tag != Tag_Segmented) return;
		validateTransform(m);
		segments.push_back(PathSegment::mkQuadCurveToPoint(cpx, cpy, x, y));

		endPoint = applyTransform(m, x, y);
	}

	inline void close() {
		if (tag != Tag_Segmented) return;
		segments.push_back(PathSegment::mkClosure());

		endPoint = startPoint;
	}
};


#endif
