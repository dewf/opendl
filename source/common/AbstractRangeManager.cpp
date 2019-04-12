#include "AbstractRangeManager.h"

#include <algorithm>
#include <map>
#include <set>
#include <assert.h>

//////public: void __thiscall AbstractRangeMananger<class cf::MutableDictionary, class cf::Dictionary>::addRange(struct dl_CFRange, class cf::Dictionary *)
////template <class A, class B>
////void AbstractRangeMananger<A,B>::addRange(dl_CFRange range, B *spec)
////{
////	RangeSpec rs;
////	rs.range = range;
////	rs.spec = spec;
////	ranges.push_back(rs);
////}
//
//struct StartEndItem {
//	unsigned int loc;   // location in layout text
//	unsigned int index; // more recent take priority (sorted later)
//	enum {
//		Start,
//		End
//	} which;
//	bool operator <(const StartEndItem &b) const {
//		if (loc == b.loc) {
//			if (index == b.index) {
//				// this really shouldn't happen, but ..
//				assert(which != b.which);
//				return which < b.which;
//			}
//			else {
//				return index < b.index;
//			}
//		}
//		else {
//			return loc < b.loc;
//		}
//	}
//};
//
//template <class A, class B>
//void AbstractRangeMananger<A,B>::apply()
//{
//	if (ranges.empty()) return;
//
//	// scan through all the ranges and create a master list of start/end points
//	std::vector<StartEndItem> locations;
//	for (unsigned int i = 0; i < ranges.size(); i++) {
//		auto &r = ranges[i];
//		locations.push_back({ dl_CFRangeEnd(r.range), i, StartEndItem::Start });
//		locations.push_back({ dl_CFRangeEnd(r.range), i, StartEndItem::End });
//	}
//
//	// put the StartEndItems in order
//	// note that that struct has its own sorting operating, first sort by .loc and then by .index
//	std::sort(locations.begin(), locations.end());
//
//	// group by common locations
//	// this defines the endpoints of segments with 0+ StartEndItems being active during the segment
//	std::map<unsigned int, std::vector<StartEndItem>> grouped;
//	for (auto iloc = locations.begin(); iloc != locations.end(); iloc++) {
//		grouped[iloc->loc].push_back(*iloc);
//	}
//	// step through each time, emit a segment with all open/active effects
//	std::set<int> active;
//	unsigned int _lastLoc;
//	unsigned int *lastLoc = nullptr; // basically trying to use std::optional without requiring C++17 everywhere
//	for (auto igrp = grouped.begin(); igrp != grouped.end(); igrp++) {
//		auto thisLoc = igrp->first;
//		if (lastLoc && active.size() > 0) {
//			// merge all the active effects for this segment + emit
//			auto accum = newAccum();
//
//			for (auto iindex = active.begin(); iindex != active.end(); iindex++) {
//				accumulate(accum, ranges[*iindex].spec);
//			}
//			// emit to subclass
//			RangeSpec rs;
//			rs.spec = accum;
//			rs.start = *lastLoc;
//			rs.length = thisLoc - *lastLoc;
//			applyRangeSpec(&rs);
//
//			releaseAccum(accum);
//		}
//		// update the active effects
//		for (auto item = igrp->second.begin(); item != igrp->second.end(); item++) {
//			if (item->which == StartEndItem::Start) {
//				active.emplace(item->index);
//			}
//			else {
//				active.erase(item->index);
//			}
//		}
//		lastLoc = &_lastLoc;
//		*lastLoc = thisLoc;
//	}
//}
