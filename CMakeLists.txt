cmake_minimum_required(VERSION 3.10)
project(custom-sdr-ftdi-tester LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TIAFE77_LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/tiafe77-cafe/lib)
set(FTDI_LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/custom-sdr-ftdi-lib)
add_subdirectory(${TIAFE77_LIB_PATH})
add_subdirectory(${FTDI_LIB_PATH})

file(GLOB_RECURSE ALL_SRC_HPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE ALL_SRC_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(custom-sdr-ftdi-tester
    ${ALL_SRC_HPP_FILES}
    ${ALL_SRC_CPP_FILES}
)

target_include_directories(custom-sdr-ftdi-tester
PUBLIC
    .
    src
    src/custom-sdr-parts
    src/custom-sdr-parts/common
    src/custom-sdr-parts/accessors
    src/custom-sdr-parts/generic-ftdi
    ${TIAFE77_LIB_PATH}
    ${FTDI_LIB_PATH}
)

target_link_libraries (custom-sdr-ftdi-tester
PUBLIC
    -dynamic
    -fPIC

    -L${CMAKE_SYSROOT}/usr/lib
    -L${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu

    -pthread

    custom-sdr-ftdi-lib
    tiafe77-cafe-lib

    -lboost_filesystem 
    -lboost_system 

)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")
endif()

install(TARGETS custom-sdr-ftdi-tester DESTINATION ${CMAKE_INSTALL_PREFIX})

