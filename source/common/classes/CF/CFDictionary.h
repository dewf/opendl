#ifndef __CF_DICTIONARY_H__
#define __CF_DICTIONARY_H__

#include "CFObject.h"

namespace cf {
	// DICTIONARY CLASS ================================
	struct CmpByObjectValue {
		bool operator()(const ObjectRef& a, const ObjectRef& b) const {
			return *a < *b; // uses the comparison operator as (hopefully) implemented by derived classes
		}
	};

	typedef void (*DictionaryApplierFunction)(const void *key, const void *value, void *context);

	class Dictionary; typedef Dictionary* DictionaryRef;
	class Dictionary : public Object {
	protected:
		std::map<ObjectRef, ObjectRef, CmpByObjectValue> dict;
	public:
		TypeID getTypeID() const override { return kTypeIDDictionary; }
		const char *getTypeName() const override { return "CFDictionary"; }

		static DictionaryRef EmptyDict;

		Dictionary() {}

		explicit Dictionary(DictionaryRef copyFrom) {
			for (auto &i : copyFrom->dict) {
				auto key = i.first->copy();
				auto value = i.second->retain();
				dict[key] = value;
			}
		}

		Dictionary(ObjectRef *keys, ObjectRef *values, dl_CFIndex numValues) {
			for (dl_CFIndex i = 0; i < numValues; i++) {
				auto key = keys[i]->copy();
				auto value = values[i]->retain();
				dict[key] = value;
			}
		}

		~Dictionary() {
			for (auto &i : dict) {
				i.first->release();
				i.second->release();
			}
		}

		dl_CFIndex getCount() {
			return (dl_CFIndex)dict.size();
		}

		ObjectRef getValue(ObjectRef key) {
			auto found = dict.find(key);
			if (found != dict.end()) {
				return found->second;
			}
			else {
				return nullptr;
			}
		}

		bool getValueIfPresent(ObjectRef key, ObjectRef *value) {
			auto found = dict.find(key);
			if (found != dict.end()) {
				*value = found->second;
				return true;
			}
			else {
				*value = nullptr;
				return false;
			}
		}

		void getKeysAndValues(ObjectRef *keys, ObjectRef *values) {
			for (auto &i : dict) {
				*keys++ = i.first;
				*values++ = i.second;
			}
		}

		void applyFunction(DictionaryApplierFunction applier, void *context) {
			for (auto &i : dict) {
				applier(i.first, i.second, context);
			}
		}

		template<typename Functor>
		void applyFunctor(Functor functor) {
			for (auto &i : dict) {
				functor(i.first, i.second);
			}
		}

		virtual DictionaryRef copy() override {
			// already immutable, just return self
			return (DictionaryRef)retain();
		}

		virtual std::string toString() const override {
			return sprintfToStdString("Dictionary@%p [%s]", this, Object::toString().c_str());
		}

		RETAIN_AND_AUTORELEASE(Dictionary);
		WEAKREF_MAKE(Dictionary);
	};
}

#endif // __CF_DICTIONARY_H__
