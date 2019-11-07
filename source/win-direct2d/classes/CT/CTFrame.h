#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include "../../private_defs.h"
#include "../CG/CGPath.h"

//#include "../CT/CTFont.h"
//#include "../../directwrite/MyTextRenderer.h"
//#include "../../directwrite/CoreTextFormatSpec.h"
//#include "../../directwrite/CoreTextRenderer.h"

#include "../CT/CTLine.h"
#include <vector>
#include "../../directwrite/DWriteLayout.h"

class CTFrame; typedef CTFrame* CTFrameRef;
class CTFrame : public cf::Object {
private:
	cf::AttributedStringRef attrString = nullptr;
	CGPathRef path = nullptr;
	cf::DictionaryRef frameAttrs = nullptr;
	DWriteLayout *layout = nullptr;
	cf::ArrayRef lines = nullptr; // of type CTLineRef
public:
	const char *getTypeName() const override { return "CTFrame"; }

	CTFrame(cf::AttributedStringRef attrString, CGPathRef path, cf::DictionaryRef frameAttrs);
	~CTFrame();

	cf::ArrayRef getLines();

	void getLineOrigins(dl_CFRange range, dl_CGPoint origins[]);

	void drawAtPosition(CGContextRef context, dl_CGPoint p) {
		layout->drawAtPosition(context, p);
	}

	void draw(CGContextRef context) {
		auto rect = path->getRect();
		drawAtPosition(context, rect.origin);
	}

	CTFrameRef copy() override {
		// immutable so return self
		return (CTFrameRef) retain();
	}

	RETAIN_AND_AUTORELEASE(CTFrame)
};
