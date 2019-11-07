#pragma once

#include "../../opendl.h"
#include "../../../deps/CFMinimal/source/CF/CFTypes.h"

#include "../private_defs.h"

#include "../classes/CG/CGColor.h"
#include "../classes/CT/CTFont.h"
#include "../classes/CT/CTParagraphStyle.h"


#include "MyUnknown.h"

class CoreTextFormatSpec : public MyUnknown
{
private:
	cf::DictionaryRef attrs;
public:
	CoreTextFormatSpec(cf::DictionaryRef attrs)
		:MyUnknown(), // nothing beyond IUnknown
		 attrs(attrs->copy()) {}
	~CoreTextFormatSpec() override {
		attrs->release();
	}
	void applyToLayout(IDWriteTextLayout *layout, DWRITE_TEXT_RANGE range) {
		layout->SetDrawingEffect(this, range); // colors: highlight, foreground, underline/strikethrough, underline count / strikethrough count, etc etc
											   // everything else we set here (non-target-dependent stuff, especially):
		CTFontRef font;
		if (attrs->getValueIfPresent((cf::ObjectRef)dl_kCTFontAttributeName, (cf::ObjectRef *)&font)) {
			font->applyToLayout(layout, range);
		}

		CTParagraphStyleRef paraStyle;
		if (attrs->getValueIfPresent((cf::ObjectRef)dl_kCTParagraphStyleAttributeName, (cf::ObjectRef *)&paraStyle)) {
			// handle paragraph styles -- unfortunately (for now) these only work when applied to the entire Layout
			// how could we handle range-specific styles on windows? do we need a layout per paragraph of text, and to store lists of layouts instead of a single one? ugly!
			auto &settings = paraStyle->settings;
			for (auto i = settings.begin(); i != settings.end(); i++) {
				switch (i->spec) {
				case dl_kCTParagraphStyleSpecifierAlignment:
					switch (i->alignment) {
					case dl_kCTTextAlignmentCenter:
						layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
						break;
					case dl_kCTTextAlignmentRight:
						layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
						break;
					case dl_kCTTextAlignmentJustified:
						layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED); // no range applicability :(
						break;
					}
					break;
				}
			}
		}
		// dl_kCTForegroundColorAttributeName -> handled as an effect in the custom renderer
		// dl_kCTStrokeWidthAttributeName -> same
		// dl_kCTStrokeColorAttributeName -> same

		// + underline/stroke/+counts/etcetc
	}
	// various accessors

	inline ID2D1SolidColorBrush *getColorBrushForKey(ID2D1RenderTarget *target, cf::StringRef key) {
		CGColorRef color;
		if (attrs->getValueIfPresent((cf::ObjectRef)key, (cf::ObjectRef*)&color)) {
			return color->getCachedBrush(target);
		}
		else {
			return nullptr;
		}
	}

	//dl_kCTForegroundColorAttributeName
	ID2D1SolidColorBrush *getForegroundBrush(ID2D1RenderTarget *target) {
		return getColorBrushForKey(target, (cf::StringRef)dl_kCTForegroundColorAttributeName);
	}

	//dl_kCTForegroundColorFromContextAttributeName (required for Core Text to use the context's fill color - otherwise defaults to black if no kDLForegroundColorAttribute specified)
	bool useContextForegroundColor() {
		cf::BooleanRef value;
		if (attrs->getValueIfPresent((cf::ObjectRef)dl_kCTForegroundColorFromContextAttributeName, (cf::ObjectRef *)&value)) {
			return value->getValue();
		}
		else {
			return false;
		}
	}

	//dl_kCTStrokeColorAttributeName
	ID2D1SolidColorBrush *getStrokeColor(ID2D1RenderTarget *target) {
		return getColorBrushForKey(target, (cf::StringRef)dl_kCTStrokeColorAttributeName);
	}

	//dl_kCTStrokeWidthAttributeName
	bool getStrokeWidth(float *strokeWidth) {
		cf::NumberRef number;
		if (attrs->getValueIfPresent((cf::ObjectRef)dl_kCTStrokeWidthAttributeName, (cf::ObjectRef*)&number)) {
			return number->getValue(kCFNumberFloatType, strokeWidth);
		}
		else {
			return false;
		}
	}

	//dl_kCTFontAttributeName
	CTFontRef getFont() {
		// note that this mainly needs to be applied to the IDWriteTextLayout before rendering (see applyToLayout method above)
		// so far no need for this, just having an accessor for completeness
		CTFontRef font;
		if (attrs->getValueIfPresent((cf::ObjectRef)dl_kCTFontAttributeName, (cf::ObjectRef *)&font)) {
			return font;
		}
		else {
			return nullptr;
		}
	}
	

	cf::DictionaryRef getAttrs() {
		// get rule
		return attrs;
	}
};

