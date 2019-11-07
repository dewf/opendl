#pragma once

#include "../../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../../opendl.h"
#include "../../private_defs.h"

#include "../CG/CGContext.h"
//#include "../../directwrite/DWriteLayout.h"
#include <vector>

class DWriteLayout;

class CTLine; typedef CTLine* CTLineRef;
class CTLine : public cf::Object {
	// ========================
	DWriteLayout *layout = nullptr;
	bool layoutOwned = false;
	// ========================
	DWRITE_LINE_METRICS lineMetrics;
	dl_CGFloat lineWidth; // actual line width (sum of run widths - not from frame / idtextwritelayout)
	cf::ArrayRef runs = nullptr; // of type CTRunRef
	dl_CFRange textRange; // char index range of the enclosing layout
public:
	const char *getTypeName() const override { return "CTLine"; }

	CTLine(cf::ArrayRef runs, dl_CGFloat lineWidth, DWRITE_LINE_METRICS *metrics, DWriteLayout *layout, bool layoutOwned = false);
	~CTLine();
	static CTLineRef createFromAttrString(cf::AttributedStringRef attrString);

	double getTypographicBounds(dl_CGFloat *ascent, dl_CGFloat *descent, dl_CGFloat *leading);

	//dl_kCTLineBoundsExcludeTypographicLeading = 1 << 0,
	//dl_kCTLineBoundsExcludeTypographicShifts = 1 << 1,
	//dl_kCTLineBoundsUseHangingPunctuation = 1 << 2,
	//dl_kCTLineBoundsUseGlyphPathBounds = 1 << 3,
	//dl_kCTLineBoundsUseOpticalBounds = 1 << 4
	dl_CGRect getBoundsWithOptions(dl_CTLineBoundsOptions options);

	void draw(CGContextRef context);

	cf::ArrayRef getGlyphRuns();

	dl_CGFloat getOffsetForStringIndex(dl_CFIndex charIndex, dl_CGFloat *secondaryOffset);
	dl_CFIndex getStringIndexForPosition(dl_CGPoint position);
	dl_CFRange getStringRange();

	dl_CGPoint getOrigin();

	virtual CTLineRef copy() {
		// immutable so return self
		return (CTLineRef)retain();
	}

	RETAIN_AND_AUTORELEASE(CTLine)
};
