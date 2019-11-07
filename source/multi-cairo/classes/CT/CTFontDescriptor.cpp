#include <utility>

#include "CTFontDescriptor.h"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

const char *CTFontDescriptor::getTypeName() const {
    return "CTFontDescriptor";
}

CTFontDescriptorRef CTFontDescriptor::copy() {
    // immutable
    return retain();
}

CTFontDescriptor::CTFontDescriptor(std::string path, int index, std::string familyName)
    :path(std::move(path)),
     index(index),
     familyName(std::move(familyName))
{
    // nothing yet
}

cf::ArrayRef CTFontDescriptor::createFontDescriptorsFromURL(cf::URLRef fileURL)
{
    static FT_Library library = nullptr;
    if (!library) {
        auto error = FT_Init_FreeType(&library);
        if (error) {
            printf("Some error initializing Freetype :(");
            return nullptr;
        }
    }
    auto path = fileURL->copyFileSystemPath(kCFURLPOSIXPathStyle)->autoRelease();
    auto strPath = path->getUtf8String();

    FT_Face face;
    auto error = FT_New_Face(library, strPath.c_str(), 0, &face);
    if (error) {
        // freetype didn't recognize the file, oh well!
        return nullptr;
    }
    std::string familyName = face->family_name;
    // close it
    FT_Done_Face(face);

    // just single files for now
    auto ctFont = new CTFontDescriptor(strPath, 0, familyName);
    auto result = cf::Array::create((cf::ObjectRef *)&ctFont, 1);
    ctFont->release();

    return result;
}

