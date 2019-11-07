#ifndef APIDEMO_MYPANGOLAYOUT_H
#define APIDEMO_MYPANGOLAYOUT_H

#include "../../../deps/CFMinimal/source/CF/CFTypes.h"
#include "../../opendl.h"
#include "CoreTextAttrs.h"
#include <pango/pango-layout.h>

struct PangoRunGeom {
    dl_CGFloat xOffset;
    dl_CGFloat width;
};

struct PangoRunStruct {
    PangoLayoutRun *run = nullptr;
    dl_CFRange stringRange = dl_CFRangeZero;
    dl_CTRunStatus runStatus = 0;
    std::vector<PangoRunGeom> offsets;
    dl_CGRect inkRect = {};
    dl_CGRect logicalRect = {};
    PangoLanguage *lang = nullptr;
    PangoFontMetrics *metrics = nullptr;
    double lineHeight = -1;
    double baseLine = -1;
    CoreTextAttrs *attrs = nullptr;
};

struct PangoLineStruct {
    PangoLayoutLine *line = nullptr;
//    dl_CFRange stringRange = dl_CFRangeZero;
    int lineIndex = -1;
    dl_CGRect inkRect, logicalRect;
    double baseline = -1;
    double spacing = -1;
    std::vector<PangoRunStruct> runs;
    dl_CGPoint origin;
};

class MyPangoLayout; typedef MyPangoLayout* MyPangoLayoutRef;
class MyPangoLayout: public cf::Object {
    PangoLayout *layout = nullptr;
    cf::AttributedStringRef attrString;
    std::vector<PangoLineStruct> lines;

    // apply the text effects from the attributed string to the actual pango layout
    // then they'll be available as properties on each run during rendering
    void applyAttributes();

    // collect all required info about lines and runs for random access
    void prescan();
public:
    bool rightToLeft = false;

    const char *getTypeName() const override;
    cf::ObjectRef copy() override;

    explicit MyPangoLayout(cf::AttributedStringRef attrString);
    ~MyPangoLayout() override;

    // both of these call prescan() to actually perform the layout
    PangoLineStruct layoutSingleLine();
    const std::vector<PangoLineStruct> layoutArea(dl_CGFloat width, dl_CGFloat height);

    dl_CGFloat getSpacing();

    PangoLayout *getRawLayout() { return layout; }

    bool isNewlineAtIndex(int index);

    RETAIN_AND_AUTORELEASE(MyPangoLayout);
    WEAKREF_MAKE(MyPangoLayout);
};

#endif //APIDEMO_MYPANGOLAYOUT_H
