cmake_minimum_required(VERSION 3.10)
project(custom-sdr-afe77xx-lib LANGUAGES C CXX)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 14)

get_filename_component(VAR_PARENT ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)
set(FTDI_LIB_PATH ${VAR_PARENT}/custom-sdr-ftdi-lib)

file(GLOB_RECURSE ALL_SRC_H_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE ALL_SRC_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE ALL_SRC_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(custom-sdr-afe77xx-lib STATIC
    ${ALL_SRC_H_FILES}
    ${ALL_SRC_C_FILES}
    ${ALL_SRC_CPP_FILES}
)

#add_subdirectory(${FTDI_LIB_PATH})

set_target_properties(custom-sdr-afe77xx-lib PROPERTIES PREFIX "")
set_target_properties(custom-sdr-afe77xx-lib PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/afe77xx.h)

target_include_directories(custom-sdr-afe77xx-lib
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/include

    ${FTDI_LIB_PATH}
)

target_link_libraries (custom-sdr-afe77xx-lib
PRIVATE
    -dynamic
    -fPIC

    -L${CMAKE_SYSROOT}/usr/lib
    -L${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu

    custom-sdr-ftdi-lib
)

add_definitions(-DLIBAFE77XX )

