cmake_minimum_required(VERSION 3.8)
project(opendl)

set(CMAKE_CXX_STANDARD 11)

Set(GCC_VISIBILITY_FLAGS "-fvisibility=hidden")
add_definitions(${GCC_VISIBILITY_FLAGS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake") # for FindCairo.cmake, FindPoppler.cmake

find_package(PkgConfig REQUIRED)
find_package(Cairo REQUIRED)
find_package(Pango REQUIRED)
find_package(ICU REQUIRED)

#pkg_check_modules(PANGOMM pangomm-1.4 REQUIRED) ${PANGOMM_INCLUDE_DIRS} ${PANGOMM_LIBRARY_DIRS} ${PANGOMM_LIBRARIES}

include_directories(${CAIRO_INCLUDE_DIRS} ${PANGO_INCLUDE_DIRS} ${ICU_INLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS} ${PANGO_LIBRARY_DIRS} ${ICU_LIBRARY_DIRS})

set(SOURCE_FILES
        "../../source/multi-cairo/opendl.cpp"
        "../../source/dlcf.h"
        "../../source/opendl.h"
        "../../source/common/dlcf.cpp"
        "../../source/common/geometry.cpp"
        "../../source/common/AbstractRangeManager.h"
        "../../source/common/AbstractRangeManager.cpp"
        "../../source/common/util.h"
        "../../source/common/util.cpp"

#        "../../source/common/classes/CF/CFArray.h"
#        "../../source/common/classes/CF/CFArray.cpp"
#        "../../source/common/classes/CF/CFAttributedString.h"
#        "../../source/common/classes/CF/CFAttributedString.cpp"
#        "../../source/common/classes/CF/CFBoolean.h"
#        "../../source/common/classes/CF/CFBoolean.cpp"
#        "../../source/common/classes/CF/CFDictionary.h"
#        "../../source/common/classes/CF/CFDictionary.cpp"
#        "../../source/common/classes/CF/CFMisc.h"
#        "../../source/common/classes/CF/CFMisc.cpp"
#        "../../source/common/classes/CF/CFMutableArray.h"
#        "../../source/common/classes/CF/CFMutableArray.cpp"
#        "../../source/common/classes/CF/CFMutableAttributedString.h"
#        "../../source/common/classes/CF/CFMutableAttributedString.cpp"
#        "../../source/common/classes/CF/CFMutableDictionary.h"
#        "../../source/common/classes/CF/CFMutableDictionary.cpp"
#        "../../source/common/classes/CF/CFMutableString.h"
#        "../../source/common/classes/CF/CFMutableString.cpp"
#        "../../source/common/classes/CF/CFNumber.h"
#        "../../source/common/classes/CF/CFNumber.cpp"
#        "../../source/common/classes/CF/CFObject.h"
#        "../../source/common/classes/CF/CFObject.cpp"
#        "../../source/common/classes/CF/CFString.h"
#        "../../source/common/classes/CF/CFString.cpp"
#        "../../source/common/classes/CF/CFTypes.h"
#        "../../source/common/classes/CF/CFURL.h"
#        "../../source/common/classes/CF/CFURL.cpp"

        "../../source/multi-cairo/classes/CG/CGColor.cpp"
        "../../source/multi-cairo/classes/CG/CGColor.h"

        "../../source/multi-cairo/pango-renderer/StringAttrManager.h"
        "../../source/multi-cairo/pango-renderer/StringAttrManager.cpp"
        "../../source/multi-cairo/pango-renderer/ExtendedAttrs.h"
        "../../source/multi-cairo/pango-renderer/ExtendedAttrs.cpp"
        "../../source/multi-cairo/classes/CT/CTFont.cpp"
        "../../source/multi-cairo/classes/CT/CTFont.h"
        "../../source/multi-cairo/util.cpp"
        "../../source/multi-cairo/util.h"
        "../../source/multi-cairo/classes/CT/CTLine.cpp"
        "../../source/multi-cairo/classes/CT/CTLine.h"
        "../../source/multi-cairo/classes/CG/CGContext.cpp"
        "../../source/multi-cairo/classes/CG/CGContext.h"
        "../../source/multi-cairo/classes/CG/CGGradient.cpp"
        "../../source/multi-cairo/classes/CG/CGGradient.h"
        "../../source/multi-cairo/classes/CG/CGColorSpace.cpp"
        "../../source/multi-cairo/classes/CG/CGColorSpace.h"
        "../../source/multi-cairo/classes/CT/CTFrameSetter.cpp"
        "../../source/multi-cairo/classes/CT/CTFrameSetter.h"
        "../../source/multi-cairo/classes/CG/CGPath.cpp"
        "../../source/multi-cairo/classes/CG/CGPath.h"
        "../../source/multi-cairo/classes/CT/CTFrame.cpp"
        "../../source/multi-cairo/classes/CT/CTFrame.h"
        "../../source/multi-cairo/pango-renderer/CoreTextAttrs.cpp"
        "../../source/multi-cairo/pango-renderer/CoreTextAttrs.h"
        "../../source/multi-cairo/classes/CT/CTRun.cpp"
        "../../source/multi-cairo/classes/CT/CTRun.h"
        "../../source/multi-cairo/pango-renderer/CoreTextAttrManager.cpp"
        "../../source/multi-cairo/pango-renderer/CoreTextAttrManager.h"
        "../../source/multi-cairo/pango-renderer/MyPangoLayout.cpp"
        "../../source/multi-cairo/pango-renderer/MyPangoLayout.h" ../../source/multi-cairo/classes/CT/CTParagraphStyle.cpp ../../source/multi-cairo/classes/CT/CTParagraphStyle.h ../../source/multi-cairo/classes/CG/CGBitmapContext.cpp ../../source/multi-cairo/classes/CG/CGBitmapContext.h ../../source/multi-cairo/classes/CG/CGImage.cpp ../../source/multi-cairo/classes/CG/CGImage.h ../../source/multi-cairo/classes/CT/CTFontDescriptor.cpp ../../source/multi-cairo/classes/CT/CTFontDescriptor.h)

add_library(opendl SHARED ${SOURCE_FILES})

add_subdirectory("../../deps/CFMinimal/build/clion/" "${CMAKE_CURRENT_BINARY_DIR}/cfminimal")

target_link_libraries(opendl cfminimal ${CAIRO_LIBRARIES} ${PANGO_LIBRARIES} ${ICU_LIBRARIES})
