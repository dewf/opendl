#ifndef __CF_MISC_H__
#define __CF_MISC_H__

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <memory>

#include <assert.h>

#include "../../../dlcf.h"
#include "../../../common/util.h"

namespace cf {

	// EXCEPTION OBJECT ================================
	class Exception : public std::exception {
		std::string reason;
		const char *what() const throw() override {
			return reason.c_str();
		}
	public:
		Exception(const char *reason) {
			this->reason = reason;
		}
	};

	typedef enum {
		kTypeIDObject,
		kTypeIDBoolean,
		kTypeIDString,
		kTypeIDMutableString,
		kTypeIDArray,
		kTypeIDMutableArray,
		kTypeIDDictionary,
		kTypeIDMutableDictionary,
		kTypeIDAttributedString,
		kTypeIDMutableAttributedString,
		kTypeIDNumber,
		kTypeIDURL
	} TypeID;

	// REFCOUNTED (common root of Object and WeakRef)
	class RefCounted; typedef RefCounted* RefCountedRef;
	class RefCounted {
	protected:
		int refCount = 1;
	public:
		bool isEternal = false; // set this flag to prevent releasing

		RefCounted(); // used for debugging memory leaks (registers itself behind the scenes)
		virtual ~RefCounted();

		static void startLeakMonitoring();
		static void endLeakMonitoring();

		virtual std::string toString() const {
			return sprintfToStdString("RefCounted @ %p: refCount %d, isEternal: %s", this, refCount, isEternal ? "true" : "false");
		}

		RefCountedRef retain() {
			refCount++;
			return this;
		}

		virtual void release(); // virtual in case of need for any special logic (unfortunately the case with CTLine for now)

		struct RefCountedReleaser {
			void operator()(RefCountedRef obj) { obj->release(); }
		};

		std::unique_ptr<RefCounted, RefCountedReleaser> autoRelease() {
			// automatically calls ->release() when it goes out of scope :)
			return std::unique_ptr<RefCounted, RefCountedReleaser>(this);
		}
	};

	template <class T>
	T makeEternal(T obj) {
		obj->isEternal = true;
		return obj;
	}

	// a macro for some stuff that's beyond my skill level with templates
#define RETAIN_AND_AUTORELEASE(x) \
	inline x##Ref retain() { \
		return (x##Ref)RefCounted::retain(); \
	} \
	inline std::unique_ptr<x, RefCountedReleaser> autoRelease() { \
		return std::unique_ptr<x, RefCountedReleaser>(this); \
	}

	// WEAKREF OBJECT ==================================
	template <class T>
	class WeakRef : public RefCounted {
		T* ref = nullptr;
	public:
		WeakRef(T* ref)
			:ref(ref) {}

		T* get() { return ref; } // holders get() before each use

		void eraseAndRelease() { // owner erases on cleanup
			ref = nullptr;
			release();
		}
	};

} // end namespace cf

#endif // __CF_MISC_H__

