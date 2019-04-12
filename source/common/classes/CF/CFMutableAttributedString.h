#ifndef __CF_MUTABLEATTRIBUTEDSTRING_H__
#define __CF_MUTABLEATTRIBUTEDSTRING_H__

#include "CFAttributedString.h"

namespace cf {
	class AttrStringRangeManager; // forward reference to friend class

	class MutableAttributedString; typedef MutableAttributedString* MutableAttributedStringRef;
	class MutableAttributedString : public AttributedString {
	private:
		void coalesce(); // merge all the queued changes in with the existing ranges
		void deleteRange(dl_CFRange delRange);
		void insertRange(dl_CFRange insRange);
		//
		struct QueuedChange {
			dl_CFRange range;
			StringRef key;
			ObjectRef value;
		};
		std::vector<QueuedChange> queuedChanges; // to apply and merge with the existing ranges
		bool isEditing = false;
	public:
		TypeID getTypeID() const override { return kTypeIDMutableAttributedString; }
		const char *getTypeName() const override { return "CFMutableAttributedString"; }

		MutableAttributedString() : AttributedString() {}

		MutableAttributedString(AttributedStringRef str) // ie, create mutable copy
			:AttributedString(*str) {}

		~MutableAttributedString() {
			// release any uncommitted changes
			for (auto i = queuedChanges.begin(); i != queuedChanges.end(); i++) {
				i->key->release();
				i->value->release();
			}
		}

		void beginEditing() {
			isEditing = true;
		}
		void endEditing() {
			if (isEditing) {
				coalesce();
				isEditing = false;
			}
		}

		void replaceString(dl_CFRange replRange, StringRef replacement);

		void setAttribute(dl_CFRange range, StringRef attrName, ObjectRef value) {
			QueuedChange qc;
			qc.range = range;
			qc.key = attrName->copy();
			qc.value = value->retain();
			queuedChanges.push_back(qc);
			if (!isEditing) {
				coalesce();
			}
		}

		virtual AttributedStringRef copy() override {
			// return immutable copy
			return new AttributedString(str, ranges);
		};

		friend class AttrStringRangeManager; // writes directly to mutable string instance

		RETAIN_AND_AUTORELEASE(MutableAttributedString)
			WEAKREF_MAKE(MutableAttributedString)
	};
}

#endif //__CF_MUTABLEATTRIBUTEDSTRING_H__