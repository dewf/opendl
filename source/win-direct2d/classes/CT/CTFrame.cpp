#include "CTFrame.h"

#include <assert.h>
#include "../../unicodestuff.h"

CTFrame::CTFrame(cf::AttributedStringRef attrString, CGPathRef path, cf::DictionaryRef frameAttrs)
	:attrString(attrString->copy()),
	 path(path->copy())
{
	if (frameAttrs) {
		this->frameAttrs = frameAttrs->copy();
	}

	// only rect supported for now
	auto rect = path->getRect();

	layout = new DWriteLayout(this->attrString);
	lines = layout->layoutArea(rect.size.width, rect.size.height);
}

CTFrame::~CTFrame()
{
	lines->release();
	::SafeRelease(&layout);
	//
	attrString->release();
	path->release();
	if (frameAttrs) {
		frameAttrs->release();
	}
}

cf::ArrayRef CTFrame::getLines()
{
	// get rule, no copy
	return lines;
}

void CTFrame::getLineOrigins(dl_CFRange range, dl_CGPoint origins[]) {
	if (range.length == 0) {
		range = dl_CFRangeMake(0, lines->getCount());
	}
	else {
		assert(range.location >= 0 && range.length >= 0 && (range.location + range.length <= lines->getCount()));
	}
	// line origins must be adjusted by frame origin (ie, absolute not relative)
	auto rect = path->getRect();

	for (int i = 0; i < range.length; i++) {
		auto line = (CTLineRef)lines->getValueAtIndex(range.location + i);
		auto lineOrigin = line->getOrigin();
		origins[i] = dl_CGPointMake(rect.origin.x + lineOrigin.x, rect.origin.y + lineOrigin.y);
	}
}
