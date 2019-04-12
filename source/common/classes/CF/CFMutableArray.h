#ifndef __CF_MUTABLEARRAY_H__
#define __CF_MUTABLEARRAY_H__

#include "CFArray.h"

namespace cf {
	class MutableArray; typedef MutableArray* MutableArrayRef;
	class MutableArray : public Array {
	public:
		TypeID getTypeID() const override { return kTypeIDMutableArray; }
		const char *getTypeName() const override { return "CFMutableArray"; }

		MutableArray() {}
		MutableArray(ObjectRef *values, dl_CFIndex numValues)
			:Array(values, numValues) {}

		void appendValue(ObjectRef value) {
			items.push_back(value->retain());
		}

		void insertValueAtIndex(dl_CFIndex idx, const ObjectRef value) {
			if (idx >= 0 && idx <= (dl_CFIndex)items.size()) {
				items.insert(items.begin() + idx, value->retain());
			}
			else {
				throw Exception("MutableArray::insertValueAtIndex() - out of range\n");
			}
		}

		void removeAllValues() {
			for (auto i = items.begin(); i != items.end(); i++) {
				(*i)->release();
			}
			items.clear();
		}

		virtual ArrayRef copy() override {
			// mutable, so we have to make an immutable copy
			return new Array(items.data(), (dl_CFIndex)items.size());
		}

		RETAIN_AND_AUTORELEASE(MutableArray)
		WEAKREF_MAKE(MutableArray)
	};
}

#endif // __CF_MUTABLEARRAY_H__
