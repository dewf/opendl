#ifndef __CF_MUTABLEDICTIONARY_H__
#define __CF_MUTABLEDICTIONARY_H__

#include "CFDictionary.h"

namespace cf {
	// MUTABLE DICTIONARY ==============================
	class MutableDictionary; typedef MutableDictionary* MutableDictionaryRef;
	class MutableDictionary : public Dictionary {
	public:
		TypeID getTypeID() const override { return kTypeIDMutableDictionary; }
		const char *getTypeName() const override { return "CFMutableDictionary"; }

		MutableDictionary() : Dictionary() {}

		explicit MutableDictionary(DictionaryRef copyFrom)
			: Dictionary(copyFrom) {}

		MutableDictionary(ObjectRef *keys, ObjectRef *values, dl_CFIndex numValues)
			: Dictionary(keys, values, numValues) {}

		void setValue(ObjectRef key, ObjectRef value) {
			auto v2 = value->retain();
			auto found = dict.find(key);
			if (found != dict.end()) {
				// DO have this key already, release current value
				found->second->release(); // OK if same object we're inserting - already retained it
				// no need to retain/copy key since we already have it
				dict[key] = v2;
			} else {
				// don't yet have this key, make a copy
				dict[key->copy()] = v2;
			}
		}

		void mergeFrom(DictionaryRef other) {
			// can't access other.dict directly (protected member of different instance)
			//   so we do the next easiest thing
			other->applyFunctor([this](ObjectRef key, ObjectRef value) {
				setValue(key, value);
			});
		}

		DictionaryRef copy() override { // returns IMMUTABLE copy
			// mutable, need to really copy things
			std::vector<ObjectRef> keys, values;
			for (auto &i : dict) {
				keys.push_back(i.first);
				values.push_back(i.second);
			}
			return new Dictionary(keys.data(), values.data(), (dl_CFIndex)dict.size());
		}

		RETAIN_AND_AUTORELEASE(MutableDictionary);
		WEAKREF_MAKE(MutableDictionary);
	};
}

#endif // __CF_MUTABLEDICTIONARY_H__

