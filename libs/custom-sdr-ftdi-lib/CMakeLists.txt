cmake_minimum_required(VERSION 3.10)
project(custom-sdr-ftdi-lib LANGUAGES C)

file(GLOB_RECURSE ALL_SRC_H_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE ALL_SRC_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")

add_library(custom-sdr-ftdi-lib STATIC
    ${ALL_SRC_H_FILES}
    ${ALL_SRC_C_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/include/i2c.h
    ${CMAKE_CURRENT_LIST_DIR}/include/spi.h
    ${CMAKE_CURRENT_LIST_DIR}/include/ftdi.h
)

set_target_properties(custom-sdr-ftdi-lib PROPERTIES PREFIX "")
set_target_properties(custom-sdr-ftdi-lib PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/spi.h)
set_target_properties(custom-sdr-ftdi-lib PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/i2c.h)
set_target_properties(custom-sdr-ftdi-lib PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/ftdi.h)

target_include_directories(custom-sdr-ftdi-lib
PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/include/ftd2xx
)

target_link_libraries (custom-sdr-ftdi-lib
PRIVATE
    -dynamic
    -fPIC

    -L${CMAKE_SYSROOT}/usr/lib
    -L${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu
    -L${CMAKE_CURRENT_LIST_DIR}/lib

    -ldl
    -pthread
)

