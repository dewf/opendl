#ifndef __CF_MUTABLESTRING_H__
#define __CF_MUTABLESTRING_H__

#include "CFString.h"

namespace cf {
	// MUTABLE STRING ==================================
	class MutableString; typedef MutableString* MutableStringRef;
	class MutableString : public String {
	public:
		TypeID getTypeID() const override { return kTypeIDMutableString; }
		const char *getTypeName() const override { return "CFMutableString"; }

		MutableString() : String() {}
		MutableString(const char *data) : String(data) {}

		void replaceString(dl_CFRange range, StringRef replacement) {
			assert(range.location >= 0);
			assert(range.length >= 0);
			assert(range.location + range.length <= (dl_CFIndex)str.length());
			auto start = str.begin() + range.location;
			auto end = start + range.length;
			str.replace(start, end, replacement->getStdString());
		}

		void replaceAll(StringRef toFind, StringRef replaceWith) {
			auto find_std = toFind->getStdString();
			auto repl_std = replaceWith->getStdString();
			auto pos = str.find(find_std);
			while (pos != str.npos) {
				str.replace(pos, find_std.length(), repl_std);
				pos = str.find(find_std, pos);
			}
		}

		virtual std::string toString() const override {
			char buffer[1024];
			snprintf(buffer, 1024, "MutableString@%p: {%s} [%s]", this, str.c_str(), Object::toString().c_str());
			return std::string(buffer);
		}

		StringRef copy() override {
			// make immutable copy
			return new String(str.c_str());
		}

		static MutableStringRef createFromString(StringRef str) {
			return new MutableString(str->getStdString().c_str());
		}

		RETAIN_AND_AUTORELEASE(MutableString)
			WEAKREF_MAKE(MutableString)
	};
}

#endif // __CF_MUTABLESTRING_H__

