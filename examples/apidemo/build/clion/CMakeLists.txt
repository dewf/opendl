cmake_minimum_required(VERSION 3.9)
project(apidemo)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        "../../source/main.cpp"
        "../../source/common.cpp"
        "../../source/page01.hpp"
        "../../source/page02.hpp"
        "../../source/page03.hpp"
        "../../source/page04.hpp"
        "../../source/page05.hpp"
        "../../source/page06.hpp"
        "../../source/page07.hpp"
        "../../source/page08.hpp")
add_executable(apidemo ${SOURCE_FILES})

add_subdirectory("../../../../build/clion" "${CMAKE_CURRENT_BINARY_DIR}/opendl") # 2nd arg is binary build dir
add_subdirectory("../../deps/openwl/build/clion" "${CMAKE_CURRENT_BINARY_DIR}/openwl") # same as above

target_link_libraries(apidemo opendl openwl)

