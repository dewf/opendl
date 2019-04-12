#pragma once

#include "../../../common/classes/CF/CFTypes.h"
#include "../../../opendl.h"
#include "../../private_defs.h"
#include "../../unicodestuff.h"

#include "../../directwrite/CoreTextFormatSpec.h"
#include "../../COMStuff.h"

class CTRun; typedef CTRun* CTRunRef;
class CTRun : public cf::Object
{
public:
	FLOAT baselineOriginX;
	FLOAT baselineOriginY;
	DWRITE_MEASURING_MODE measuringMode;
	// note these next two are STRUCTS not pointers
	DWRITE_GLYPH_RUN glyphRun;
	DWRITE_GLYPH_RUN_DESCRIPTION glyphRunDescription;
	// ======
	IUnknown* clientDrawingEffect;
	// =================================================

	const char *getTypeName() const override { return "CTRun"; }

	CTRun(FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		DWRITE_GLYPH_RUN const *glyphRun,
		DWRITE_GLYPH_RUN_DESCRIPTION const *glyphRunDescription,
		IUnknown* clientDrawingEffect)
		:baselineOriginX(baselineOriginX),
		baselineOriginY(baselineOriginY),
		measuringMode(measuringMode),
		glyphRun(*glyphRun),
		glyphRunDescription(*glyphRunDescription),
		clientDrawingEffect(clientDrawingEffect) // still need to AddRef
	{
		glyphRun->fontFace->AddRef();
		// see if we can get away with assuming the localeName is constant somewhere ...
		//const_cast<DWRITE_GLYPH_RUN_DESCRIPTION *>(glyphRunDescription)->localeName = _wcsdup(glyphRunDescription->localeName);
		if (clientDrawingEffect) {
			clientDrawingEffect->AddRef();
		}
	}

	~CTRun() {
		SafeRelease(&glyphRun.fontFace);
		//free((void *)glyphRunDescription.localeName);
		SafeRelease(&clientDrawingEffect);
	}

	dl_CFRange getStringRange() {
		return dl_CFRangeMake(glyphRunDescription.textPosition, glyphRunDescription.stringLength);
	}

	dl_CGFloat getOffsetForCharIndex(dl_CFIndex charIndex) {
		assert(charIndex >= 0 && charIndex < (dl_CFIndex)glyphRunDescription.stringLength);
		auto glyphIndex = glyphRunDescription.clusterMap[charIndex];
		return (dl_CGFloat)(baselineOriginX + glyphRun.glyphOffsets[glyphIndex].advanceOffset);
	}

	dl_CGPoint getXY() {
		return dl_CGPointMake(
			baselineOriginX,
			baselineOriginY
		);
	}

	cf::DictionaryRef getAttributes() {
		// get rule, this doesn't retain / copy
		if (clientDrawingEffect) {
			return ((CoreTextFormatSpec *)clientDrawingEffect)->getAttrs();
		}
		else {
			return cf::Dictionary::EmptyDict;
		}
	}

	double getTypographicBounds(dl_CFRange range, dl_CGFloat* ascent, dl_CGFloat* descent, dl_CGFloat* leading)
	{
		DWRITE_FONT_METRICS metrics;
		glyphRun.fontFace->GetMetrics(&metrics);
		if (ascent) {
			*ascent = (metrics.ascent / (FLOAT)metrics.designUnitsPerEm) * glyphRun.fontEmSize;
		}
		if (descent) {
			*descent = (metrics.descent / (FLOAT)metrics.designUnitsPerEm) * glyphRun.fontEmSize;
		}
		if (leading) {
			*leading = ((metrics.ascent + metrics.descent + metrics.lineGap) / (FLOAT)metrics.designUnitsPerEm) * glyphRun.fontEmSize;
		}
		if (range.length == 0) {
			range.length = glyphRun.glyphCount;
		}
		double ret = 0.0;
		if (range.location >= 0 && (range.location + range.length) <= (dl_CFIndex)glyphRun.glyphCount) {
			for (int i = 0; i < range.length; i++) {
				ret += glyphRun.glyphAdvances[i]; // should be in DIPs already
			}
		}
		return ret;
	}

	dl_CTRunStatus getStatus() {
		//	dl_kCTRunStatusNoStatus = 0,
		//	dl_kCTRunStatusRightToLeft = (1 << 0),
		//	dl_kCTRunStatusNonMonotonic = (1 << 1),
		//	dl_kCTRunStatusHasNonIdentityMatrix = (1 << 2)
		dl_CTRunStatus ret;
		ret = ((glyphRun.bidiLevel % 2) == 1) ? dl_kCTRunStatusRightToLeft : dl_kCTRunStatusNoStatus;
		return ret;
	}

	virtual CTRunRef copy() override {
		// immutable so retain
		return (CTRunRef)retain();
	}

	RETAIN_AND_AUTORELEASE(CTRun)
};

