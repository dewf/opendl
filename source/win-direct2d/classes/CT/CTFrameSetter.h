#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include "../../private_defs.h"

#include "../CG/CGPath.h"
#include "CTFrame.h"

class CTFrameSetter; typedef CTFrameSetter* CTFrameSetterRef;
class CTFrameSetter : public cf::Object {
private:
	cf::AttributedStringRef attrString;
public:
	const char *getTypeName() const override { return "CTFrameSetter"; }

	CTFrameSetter(cf::AttributedStringRef attrString)
		:attrString(attrString->copy()) {}
	inline static CTFrameSetterRef createWithAttributedString(cf::AttributedStringRef string) {
		return new CTFrameSetter(string);
	}
	~CTFrameSetter() {
		attrString->release();
	}
	CTFrameRef createFrame(CFRange stringRange, CGPathRef path, cf::DictionaryRef frameAttrs) {
		cf::AttributedStringRef sub;
		if (stringRange.length == 0) {
			sub = attrString; // full string
		}
		else {
			sub = cf::AttributedString::createWithSubstring(attrString, stringRange);
		}
		return new CTFrame(sub, path, frameAttrs);
	}

	virtual CTFrameSetterRef copy() {
		// immutable so retain
		return (CTFrameSetterRef) retain();
	}

	RETAIN_AND_AUTORELEASE(CTFrameSetter)
};

//OPENDL_API dl_CTFramesetterRef CDECL dl_CTFramesetterCreateWithAttributedString(dl_CFAttributedStringRef string);
//OPENDL_API dl_CTFrameRef CDECL dl_CTFramesetterCreateFrame(dl_CTFramesetterRef framesetter,
//	dl_CFRange stringRange, dl_CGPathRef path, dl_CFDictionaryRef frameAttributes);
