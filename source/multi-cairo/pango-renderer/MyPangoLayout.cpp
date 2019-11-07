#include "MyPangoLayout.h"
#include "../classes/CT/CTFont.h"
#include "CoreTextAttrs.h"

#include "../classes/CT/CTParagraphStyle.h"

#include <unicode/unistr.h>
#include <unicode/ubidi.h>

const char *MyPangoLayout::getTypeName() const {
    return "MyPangoLayout";
}

cf::ObjectRef MyPangoLayout::copy() {
    throw cf::Exception("copy() not supported for MyPangoLayout");
}

MyPangoLayout::MyPangoLayout(cf::AttributedStringRef attrString)
    :attrString(attrString->copy())
{
    layout = pango_layout_new(CTFont::defaultPangoContext);

    auto str = attrString->getString()->getUtf8String();
    pango_layout_set_text(layout, str.c_str(), -1);
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);

    auto fontDesc = CTFont::defaultFont->getDescription();
    pango_layout_set_font_description(layout, fontDesc);

    // need the base direction for various bounds calculations and such
    auto text = icu::UnicodeString::fromUTF8(str.c_str());
    auto dir = ubidi_getBaseDirection(text.getBuffer(), text.length());
    if (dir == UBIDI_RTL) {
        rightToLeft = true;
    }

    applyAttributes();
}

MyPangoLayout::~MyPangoLayout() {
    g_object_unref(layout);
    attrString->release();
}

void MyPangoLayout::applyAttributes() {
    // apply any "global" (attributes that apply to all ranges / the whole block)
    auto ranges1 = attrString->getRanges();
    if (!ranges1.empty()) {
        // only need first range for this (it should be applied to all but doesn't matter)
        auto first = ranges1.back();
        first.attrs->applyFunctor([this](cf::ObjectRef key, cf::ObjectRef value) {
            // since we can't do a switch ...
            if (key == (cf::ObjectRef)dl_kCTParagraphStyleAttributeName) {
                auto paraStyle = (CTParagraphStyleRef)value;
                for (auto &s: paraStyle->getSettings()) {
                    switch(s.spec) {
                        case dl_kCTParagraphStyleSpecifierAlignment:
                            {
                                assert(s.valueSize == sizeof(dl_CTTextAlignment));
                                auto alignment = *((dl_CTTextAlignment *)s.value);
                                switch(alignment) {
                                    case dl_kCTTextAlignmentJustified:
                                        pango_layout_set_justify(layout, true);
                                        break;
                                    default:
                                        break;
                                }
                            }
                            break;
                        default:
                            // nothing yet
                            break;
                    }
                }
            } // end dl_kCTParagraphStyleAttributeName
        });
    }

    // now apply all range-based attributes
    // ====================================
    auto attrList = pango_attr_list_new();

    auto ranges2 = attrString->getRanges();
    for (auto &i : ranges2) {
        // only apply the things that the pango layout needs to know about to select glyphs
        // other things (like underline style, highlight color, etc) we'll unpack ourselves during rendering
        CTFontRef font;
        if (i.attrs->getValueIfPresent((cf::ObjectRef)dl_kCTFontAttributeName, (cf::ObjectRef  *)&font)) {
            auto desc = pango_attr_font_desc_new(font->getDescription());
            desc->start_index = (guint) i.range.location;
            desc->end_index = (guint)(i.range.location + i.range.length);
            pango_attr_list_insert(attrList, desc);
        }
        // for everything else, place into the CoreTextAttrs (similar to DirectWrite effect)
        auto ctAttrs = new CoreTextAttrs(i.attrs);
        ctAttrs->start_index = (guint)i.range.location;
        ctAttrs->end_index = (guint)(i.range.location + i.range.length);
        pango_attr_list_insert(attrList, ctAttrs);
    }
    pango_layout_set_attributes(layout, attrList);
    pango_attr_list_unref(attrList);
}

static inline void pangoToDLRect(PangoRectangle *rect, dl_CGRect *drect) {
    drect->origin.x = pango_units_to_double(rect->x);
    drect->origin.y = pango_units_to_double(rect->y);
    drect->size.width = pango_units_to_double(rect->width);
    drect->size.height = pango_units_to_double(rect->height);
}

static inline void getLineExtents(PangoLayoutIter *iter, dl_CGRect *inkRect, dl_CGRect *logicalRect)
{
    PangoRectangle ink_exts, logical_exts;
    pango_layout_iter_get_line_extents(iter, &ink_exts, &logical_exts);
    if (inkRect) {
        pangoToDLRect(&ink_exts, inkRect);
    }
    if (logicalRect) {
        pangoToDLRect(&logical_exts, logicalRect);
    }
}

static inline void getRunExtents(PangoLayoutIter *iter, dl_CGRect *inkRect, dl_CGRect *logicalRect)
{
    PangoRectangle ink_exts, logical_exts;
    pango_layout_iter_get_run_extents(iter, &ink_exts, &logical_exts);
    if (inkRect) {
        pangoToDLRect(&ink_exts, inkRect);
    }
    if (logicalRect) {
        pangoToDLRect(&logical_exts, logicalRect);
    }
}

static void printRect(dl_CGRect r, const char *label) {
    printf("%s: %.2f,%.2f:%.2f*%.2f\n", label, r.origin.x, r.origin.y, r.size.width, r.size.height);
}

void MyPangoLayout::prescan()
{
    //guint charIndex = 0; // use to keep track of the string ranges represented by each run

    auto iter1 = pango_layout_get_iter(layout);
    // lineStruct loop
    do {
        PangoLineStruct ls = {};
        ls.line = pango_layout_iter_get_line_readonly(iter1);
        //auto para_baseline = pango_units_to_double(pango_layout_iter_get_baseline(iter1));

//        ls.stringRange.location = charIndex; // length will be filled in after all the runs, before lineStruct gets pushed to vector
        ls.line = pango_layout_iter_get_line_readonly(iter1);
        ls.lineIndex = (int)lines.size();
        getLineExtents(iter1, &ls.inkRect, &ls.logicalRect);
        auto abs_baseline = pango_units_to_double(pango_layout_iter_get_baseline(iter1));
        ls.baseline = abs_baseline - ls.logicalRect.origin.y;
        ls.spacing = pango_units_to_double(pango_layout_get_spacing(layout));

        // proper line origin / relative bounds
        ls.origin = dl_CGPointMake(ls.logicalRect.origin.x, abs_baseline);
        ls.logicalRect.origin.x -= ls.origin.x;
        ls.logicalRect.origin.y -= ls.origin.y; // should make this negative
        ls.inkRect.origin.x -= ls.origin.x;
        ls.inkRect.origin.y -= ls.origin.y;

        // haven't yet fixed up the run rects below ...

        // runs loop
        do {
            PangoRunStruct rs = {};
            rs.baseLine = ls.baseline;
            rs.run = pango_layout_iter_get_run_readonly(iter1);
            if (rs.run) {
                // got a run
                getRunExtents(iter1, &rs.inkRect, &rs.logicalRect); //&rs.inkRect

                // replace logical rect height with lineStruct height, so it's consistent across a lineStruct
                rs.logicalRect.size.height = ls.logicalRect.size.height;

                rs.stringRange = dl_CFRangeMake(rs.run->item->offset, rs.run->item->length);
//                dl_CFRangeMake(charIndex, rs.run->item->length); //rs.run->item->num_chars
//                charIndex += rs.run->item->length; //rs.run->item->num_chars;
                //printf("run item offset: %d length %d\n", rs.run->item->offset, rs.run->item->length);

                // assuming odd level = right-to-left (just guessing)
                rs.runStatus = (rs.run->item->analysis.level % 2) ? dl_kCTRunStatusRightToLeft : dl_kCTRunStatusNoStatus;

                rs.lang = pango_script_get_sample_language((PangoScript) rs.run->item->analysis.script);
                rs.metrics = pango_font_get_metrics(rs.run->item->analysis.font, rs.lang);

                auto desc = pango_font_describe(rs.run->item->analysis.font);
//                auto ul_thick_x = pango_units_to_double(pango_font_metrics_get_underline_thickness(rs.metrics));
//                auto famalam = pango_font_description_get_family(desc);

                double xOffsTotal = rs.logicalRect.origin.x;
                auto &glyphs = rs.run->glyphs;
                for (int i=0; i< glyphs->num_glyphs; i++) {
                    double xOffs = xOffsTotal + pango_units_to_double(glyphs->glyphs[i].geometry.x_offset);
                    double width = pango_units_to_double(glyphs->glyphs[i].geometry.width);
                    PangoRunGeom rg = {xOffs, width};
                    rs.offsets.push_back(rg);
                    //
                    xOffsTotal += width;
                }

                for (auto item = rs.run->item->analysis.extra_attrs; item != NULL; item = item->next) {
                    auto attr = (PangoAttribute *) item->data;
                    if (attr->klass->type == CoreTextAttrs::pangoAttrType()) {
                        assert(rs.attrs == nullptr); // no overwriting
                        rs.attrs = (CoreTextAttrs *)attr;
                        break; // don't care about any other type
                    }
                }

                rs.lineHeight = ls.logicalRect.size.height;
                //pango_font_metrics_unref(rs.metrics); // now in dtor for RunStruct
                ls.runs.push_back(rs);
            } else {
                // end of lineStruct
                break;
            }
        } while (pango_layout_iter_next_run(iter1));

        // all runs complete, end of lineStruct stuff

//        // complete the string range for this lineStruct, now that we know how long it is
//        ls.stringRange.length = charIndex - ls.stringRange.location;

        lines.push_back(ls);
    } while(pango_layout_iter_next_line(iter1));
    pango_layout_iter_free(iter1);
}

PangoLineStruct MyPangoLayout::layoutSingleLine()
{
    prescan();
    return lines[0];
}

const std::vector<PangoLineStruct> MyPangoLayout::layoutArea(dl_CGFloat width, dl_CGFloat height)
{
    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_width(layout, pango_units_from_double(width));
    pango_layout_set_height(layout, pango_units_from_double(height));
    prescan();
    return lines;
}

dl_CGFloat MyPangoLayout::getSpacing() {
    return pango_units_to_double(pango_layout_get_spacing(layout));
}

bool MyPangoLayout::isNewlineAtIndex(int index) {
    auto str = attrString->getString()->getUtf8String();
    if (index < str.length()) {
        return str.c_str()[index] == '\n';
    } else {
        return false;
    }
}

