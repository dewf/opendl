#include "CFMutableAttributedString.h"

#include "CFMutableString.h"
#include "CFMutableDictionary.h"

#include "../../AbstractRangeManager.h"

using namespace cf;

enum RangeOverlapType {
	AEntirelyBeforeB,   //End(A) <= Start(B)
	AEntirelyAfterB,    //Start(A) >= End(B)
	AOverlapsBStart,    //Start(A) <= Start(B) && End(A) < End(B)
	AOverlapsBEnd,      //Start(A) > Start(B) && End(A) >= End(B)
	AEntirelyInsideB,   //Start(A) > Start(B) && End(A) < End(B)
	AEntirelyContainsB,  //Start(A) <= Start(B) && End(A) => End(B)
	Error
};
RangeOverlapType GetRangeOverlap(dl_CFRange a, dl_CFRange b)
{
	if (dl_CFRangeEnd(a) <= b.location) {
		return AEntirelyBeforeB;
	}
	else if (a.location >= dl_CFRangeEnd(b)) {
		return AEntirelyAfterB;
	}
	else if (a.location <= b.location && dl_CFRangeEnd(a) < dl_CFRangeEnd(b)) {
		return AOverlapsBStart;
	}
	else if (a.location > b.location && dl_CFRangeEnd(a) >= dl_CFRangeEnd(b)) {
		return AOverlapsBEnd;
	}
	else if (a.location > b.location && dl_CFRangeEnd(a) < dl_CFRangeEnd(b)) {
		return AEntirelyInsideB;
	}
	else if (a.location <= b.location && dl_CFRangeEnd(a) >= dl_CFRangeEnd(b)) {
		return AEntirelyContainsB;
	}
	// should never get here
	assert(false);
	return Error;
}

void MutableAttributedString::deleteRange(dl_CFRange delRange) {
	// first delete and adjust lengths of any ranges overlapping with the range param
	auto oldRanges = ranges;
	ranges.clear();
	for (auto i = oldRanges.begin(); i != oldRanges.end(); i++) {
		switch (GetRangeOverlap(delRange, i->range)) {
		case AEntirelyBeforeB:
		case AEntirelyAfterB: {
			// do nothing
			break;
		}
		case AOverlapsBStart: {
			// push range's beginning
			auto bEnd = dl_CFRangeEnd(i->range);
			auto bStart = dl_CFRangeEnd(delRange);
			i->range.location = bStart;
			i->range.length = bEnd - bStart;
			break;
		}
		case AOverlapsBEnd: {
			// remove range's ending
			i->range.length = delRange.location - i->range.location;
			break;
		}
		case AEntirelyInsideB: {
			// shrink length
			i->range.length -= delRange.length;
			break;
		}
		case AEntirelyContainsB: {
			i->attrs->release();
			continue; // delete entirely (skip the push_back below)
			break;
		}
		}
		ranges.push_back(*i);
	}
	// collapse gaps
	dl_CFIndex index = 0;
	for (auto i = ranges.begin(); i != ranges.end(); i++) {
		i->range.location = index;
		index += i->range.length;
	}
}

void MutableAttributedString::insertRange(dl_CFRange insRange) {
	// find which range contains the insert location, and add the insert length to it
	// then fixup the loc of all the ranges after it
	bool postInsert = false;
	dl_CFIndex index = 0;
	for (auto i = ranges.begin(); i != ranges.end(); i++) {
		if (insRange.location >= i->range.location &&
			insRange.location <= dl_CFRangeEnd(i->range))
		{
			i->range.length += insRange.length;
			postInsert = true;
		}
		else if (postInsert) {
			i->range.location = index;
		}
		index += i->range.length;
	}
}

void MutableAttributedString::replaceString(dl_CFRange replRange, StringRef replacement) {
	if (isEditing) {
		// sorry, have to do it
		coalesce();
		// ... but continue until the client code formally calls endEditing()
	}
	//
	auto mut = str->createMutableCopy();
	mut->replaceString(replRange, replacement);
	str->release();
	str = mut->copy();
	mut->release();
	deleteRange(replRange);
	insertRange(dl_CFRangeMake(replRange.location, replacement->getLength()));
}

// helper class to manage attributed range updates
class cf::AttrStringRangeManager : public AbstractRangeMananger<MutableDictionary, Dictionary>
{
private:
	MutableAttributedStringRef target;
public:
	AttrStringRangeManager(MutableAttributedStringRef target) : target(target) {}

	MutableDictionaryRef newAccum() override {
		return new MutableDictionary();
	};
	void releaseAccum(MutableDictionaryRef m) override {
		m->release();
	};
	void accumulate(MutableDictionaryRef accum, DictionaryRef item) override {
		auto count = item->getCount();
		auto keys = new ObjectRef[count];
		auto values = new ObjectRef[count];
		item->getKeysAndValues(keys, values);
		// merge keys and values from item, overwriting anything in accum
		for (int i = 0; i < count; i++) {
			accum->setValue(keys[i], values[i]);
		}
		delete keys;
		delete values;
	};

	void applyRangeSpec(RangeSpec *rangeSpec) override {
		// do something with the now non-overlapping range
		// note that the dict (rangeSpec->spec) needs to be retained/copied, as the calling code will release it right after we return
		AttributedString::RangeAttrs ra;
		ra.range = rangeSpec->range;
		ra.attrs = rangeSpec->spec->copy(); // it's a mutable dict coming in, so make an immutable copy for safety
		target->ranges.push_back(ra);
	};

	void apply() {
		target->ranges.clear();
		AbstractRangeMananger::apply();
	}
};


void MutableAttributedString::coalesce()
{
	AttrStringRangeManager manager(this);

	std::vector<DictionaryRef> toFree;

	// dump the existing ranges to the manager
	for (auto i = ranges.begin(); i != ranges.end(); i++) {
		manager.addRange(i->range, i->attrs);
		// note that it knows nothing of ownership, they are still ours to free
		toFree.push_back(i->attrs);
	}

	// then all the new stuff
	for (auto i = queuedChanges.begin(); i != queuedChanges.end(); i++) {
		auto dict = new Dictionary((ObjectRef *)&i->key, &i->value, 1);
		manager.addRange(i->range, dict);
		toFree.push_back(dict);
		// can release the key/value now, the dictionary is holding the ref
		i->key->release();
		i->value->release();
	}
	queuedChanges.clear();

	// do the magic!
	manager.apply(); // the ranges will be cleared inside here

	// free all the dictionaries that were marked for releasing (the old ranges + the one-off dictionaries we just created)
	for (auto i = toFree.begin(); i != toFree.end(); i++) {
		(*i)->release();
	}
}

// API Methods ==========================================
OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutable(dl_CFIndex maxLength)
{
	return (dl_CFMutableAttributedStringRef) new MutableAttributedString();
}

OPENDL_API dl_CFMutableAttributedStringRef CDECL dl_CFAttributedStringCreateMutableCopy(dl_CFIndex maxLength, dl_CFAttributedStringRef aStr)
{
	return (dl_CFMutableAttributedStringRef) new MutableAttributedString((AttributedStringRef)aStr);
}

OPENDL_API void CDECL dl_CFAttributedStringReplaceString(dl_CFMutableAttributedStringRef aStr, dl_CFRange range, dl_CFStringRef replacement)
{
	((MutableAttributedStringRef)aStr)->replaceString(range, (StringRef)replacement);
}

OPENDL_API void CDECL dl_CFAttributedStringSetAttribute(dl_CFMutableAttributedStringRef aStr, dl_CFRange range, dl_CFStringRef attrName, dl_CFTypeRef value)
{
	((MutableAttributedStringRef)aStr)->setAttribute(range, (StringRef)attrName, (ObjectRef)value);
}

OPENDL_API void CDECL dl_CFAttributedStringBeginEditing(dl_CFMutableAttributedStringRef aStr)
{
	((MutableAttributedStringRef)aStr)->beginEditing();
}

OPENDL_API void CDECL dl_CFAttributedStringEndEditing(dl_CFMutableAttributedStringRef aStr)
{
	((MutableAttributedStringRef)aStr)->endEditing();
}
