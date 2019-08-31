#ifndef __CF_ATTRIBUTEDSTRING_H__
#define __CF_ATTRIBUTEDSTRING_H__

#include "CFObject.h"
#include "CFString.h"
#include "CFDictionary.h"

namespace cf {
	class AttributedString; typedef AttributedString* AttributedStringRef;
	class AttributedString : public Object {
	public:
		struct RangeAttrs {
			dl_CFRange range;
			DictionaryRef attrs;
		};
	protected:
		StringRef str;
		std::vector<RangeAttrs> ranges; // non-overlapping sequence of ranges
	public:
		TypeID getTypeID() const override { return kTypeIDAttributedString; }
		const char *getTypeName() const override { return "CFAttributedString"; }

		AttributedString()
			:str(new String()) {}

		AttributedString(const AttributedString &b)
			:str(b.str->copy()),
			ranges(b.ranges)
		{
			for (auto i = ranges.begin(); i != ranges.end(); i++) {
				i->attrs->retain();
			}
		}

		// createWithSubstring
		AttributedString(AttributedStringRef source, dl_CFRange range)
			:str(String::createWithSubstring(source->str, range))
		{
			if (range.length != 0 || range.length != str->getLength()) {
				throw Exception("AttributedString constructor: substring ranges not yet supported");
				// to implement, will need to iterate over the dict ranges and keep what falls within
				// shouldn't be tooo difficult
			}
		}
		inline static AttributedStringRef createWithSubstring(AttributedStringRef source, dl_CFRange range) {
			return new AttributedString(source, range);
		}

		AttributedString(StringRef str, DictionaryRef attrs)
			:str(str->copy())
		{
			// todo: need to verify the key type of the dictionary ...
			// create a single range covering the entire string length, with a copy of the dictionary
			RangeAttrs dr;
			dr.range = dl_CFRangeMake(0, str->getLength());
			dr.attrs = attrs->copy();
			ranges.push_back(dr);
		}

		// ideally this constructor would only be visible to derived classes ...
		AttributedString(StringRef str, std::vector<RangeAttrs> inRanges)
			:str(str->copy())
		{
			for (auto i = inRanges.begin(); i != inRanges.end(); i++) {
				RangeAttrs ra;
				ra.range = i->range;
				ra.attrs = i->attrs->copy();
				ranges.push_back(ra);
			}
		}

		~AttributedString() {
			str->release();
			for (auto i = ranges.begin(); i != ranges.end(); i++) {
				i->attrs->release();
			}
		}

		const StringRef getString() const { return str; }

		dl_CFIndex getLength() const { return str->getLength(); }

		std::vector<RangeAttrs> getRanges() {
			return ranges;
		}

		virtual AttributedStringRef copy() override {
			// immutable, just return self
			return (AttributedStringRef)retain();
		}

		virtual std::string toString() const override {
			return sprintfToStdString("AttributedString@%p: [%s]", this, Object::toString().c_str());
		}

		void dump() {
			printf("AttrString: [%s]\n", str->toString().c_str());
			for (auto &range : ranges) {
				printf(" - range [%td,%td)\n", range.range.location, dl_CFRangeEnd(range.range));
				auto count = range.attrs->getCount();
				auto keys = new ObjectRef[count];
				auto values = new ObjectRef[count];
				range.attrs->getKeysAndValues(keys, values);
				for (int j = 0; j < count; j++) {
					printf("  = %s: %s\n", keys[j]->toString().c_str(), values[j]->toString().c_str());
				}
				delete[] keys;
				delete[] values;
			}
		}

		RETAIN_AND_AUTORELEASE(AttributedString)
		WEAKREF_MAKE(AttributedString)
	};
}

#endif // __CF_ATTRIBUTEDSTRING_H__