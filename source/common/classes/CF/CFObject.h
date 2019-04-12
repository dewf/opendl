#ifndef __CF_OBJECT_H__
#define __CF_OBJECT_H__

#include "CFMisc.h"

namespace cf {

	// ROOT OBJECT =====================================
	class Object; typedef Object* ObjectRef;
	class Object : public RefCounted {
		std::vector<WeakRef<Object> *> weakRefs;
	public:
		virtual TypeID getTypeID() const { return kTypeIDObject; }
		virtual const char *getTypeName() const = 0; // must implement or throw

		~Object() override {
			for (auto i = weakRefs.begin(); i != weakRefs.end(); i++) {
				(*i)->eraseAndRelease();
			}
		}

		WeakRef<Object> *newWeakRef() {
			auto ret = new WeakRef<Object>(this);
			weakRefs.push_back(ret);
			ret->retain(); // pre-retained for client (there's no sense in requiring the caller to do it, since the whole point is to keep it)
			return ret;
		}

		std::string toString() const override {
			return sprintfToStdString("CFObject[%s] @ %p: refCount %d, isEternal: %s", getTypeName(), this, refCount, isEternal ? "true" : "false");
		}

		void dump() {
			printf("%s\n", toString().c_str());
		}

		virtual ObjectRef copy() = 0; // must implement or throw

		virtual bool operator <(const Object &b) const {
			auto msg = sprintfToStdString("operator < not defined for class %s", getTypeName());
			throw Exception(msg.c_str());
		}
		bool operator ==(const Object &b) const {
			return !(*this < b) && !(b < *this);
		}
		bool operator >(const Object &b) const {
			return b < *this;
		}

		RETAIN_AND_AUTORELEASE(Object)
	};

#define WEAKREF_MAKE(x) \
		cf::WeakRef<x> *newWeakRef() { \
			return (cf::WeakRef<x> *)cf::Object::newWeakRef(); \
		}

} // end namespace cf

#define OBJECT_BASED(x) \
		class x; typedef x* x##Ref; \
		class x : cf::Object

#endif // __CF_OBJECT_H__


