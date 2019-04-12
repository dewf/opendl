#ifndef __CF_STRING_H__
#define __CF_STRING_H__

#include "CFObject.h"

namespace cf {

	// STRING CLASS ====================================
	class MutableString; typedef MutableString* MutableStringRef; // fwd decl
	class String; typedef String* StringRef;
	class String : public Object {
	protected:
		std::string str;
	public:
		TypeID getTypeID() const override { return kTypeIDString; }
		const char *getTypeName() const override { return "CFString"; }

		String() {}

		// createWithCString
		String(const char *cStr) {
			str = cStr;
		}
		inline static StringRef createWithCString(const char *cStr) {
			return new String(cStr);
		}

		// createWithSubstring
		String(StringRef source, dl_CFRange range) {
			str = source->str.substr(range.location, range.length);
		}
		inline static StringRef createWithSubstring(StringRef source, dl_CFRange range) {
			return new String(source, range);
		}

		dl_CFIndex getLength() { return (dl_CFIndex)str.length(); }

		dl_CFRange find(StringRef toFind, dl_CFStringCompareFlags compareOptions);

		std::string getStdString() {
			return str;
		}

		virtual bool operator <(const Object &b) const override {
			auto other_str = (String &)b;
			return str.compare(other_str.str) < 0;
		}

		virtual std::string toString() const override {
			return sprintfToStdString("String@%p: {%s} [%s]", this, str.c_str(), Object::toString().c_str());
		}

		virtual StringRef copy() override {
			// this is immutable, so just return itself
			return (StringRef)retain();
		}

		MutableStringRef createMutableCopy(); // defined in cpp because of circular dependencies

		static StringRef makeConstantString(const char *cStr); // defined in the .cpp file because of some static variable stuff we don't expose here (annoying compiler warnings)

		RETAIN_AND_AUTORELEASE(String)
			WEAKREF_MAKE(String)
	};

} // end namespace cf

#endif // __CF_STRING_H__
