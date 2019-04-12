#ifndef __CF_URL_H__
#define __CF_URL_H__

#include "CFObject.h"
#include "CFString.h"
#include "CFMutableString.h"

namespace cf {
	// URL =============================================
	class URL; typedef URL* URLRef;
	class URL : public Object {
	private:
		StringRef filePath;
		dl_CFURLPathStyle pathStyle;
		bool isDirectory;

		static StringRef backslash;
		static StringRef fwdslash;
	public:
		TypeID getTypeID() const override { return kTypeIDURL; }
		const char *getTypeName() const override { return "CFURL"; }

		URL(StringRef filePath, dl_CFURLPathStyle pathStyle, bool isDirectory)
			:pathStyle(pathStyle),
			isDirectory(isDirectory)
		{
			if (pathStyle == dl_kCFURLPOSIXPathStyle) {
				// we're good, same as internal format
				this->filePath = filePath->copy();
			}
			else if (pathStyle == dl_kCFURLWindowsPathStyle) {
				auto _mutable = filePath->createMutableCopy();
				_mutable->replaceAll(backslash, fwdslash);
				this->filePath = _mutable->copy();
				_mutable->release();
			}
			else {
				throw Exception("unsupported URL type");
			}
		}

		static URLRef createWithFileSystemPath(StringRef filePath, dl_CFURLPathStyle pathStyle, bool isDirectory) {
			return new URL(filePath, pathStyle, isDirectory);
		}

		~URL() {
			filePath->release();
		}

		StringRef copyFileSystemPath(dl_CFURLPathStyle pathStyle) {
			if (pathStyle == dl_kCFURLWindowsPathStyle) {
				// convert back, we store internally as POSIX
				auto _mutable = filePath->createMutableCopy();
				_mutable->replaceAll(fwdslash, backslash);
				auto ret = _mutable->copy();
				_mutable->release();
				return ret;
			}
			else if (pathStyle == dl_kCFURLPOSIXPathStyle) {
				// nothing to do, matches internal format
				return filePath->copy();
			}
			else {
				throw Exception("unhandled pathStyle in copyFileSystemPath");
			}
		}

		virtual URLRef copy() override {
			// immutable, just return self
			return (URLRef)retain();
		}

		virtual bool operator <(const Object &b) const override {
			auto otherURL = (const URL&)b;
			return *filePath < *otherURL.filePath;
		}

		virtual std::string toString() const override {
			return sprintfToStdString("URL@%p: {%s} [%s]", this, filePath->getStdString().c_str(), Object::toString().c_str());
		}

		RETAIN_AND_AUTORELEASE(URL)
			WEAKREF_MAKE(URL)
	};
	// =================================================
}

#endif // __CF_URL_H__

