cmake_minimum_required(VERSION 3.10)
project(custom-sdr-ftdi-tester LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FTDI_LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/custom-sdr-ftdi-lib)

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
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")
endif()

install(TARGETS custom-sdr-ftdi-tester DESTINATION ${CMAKE_INSTALL_PREFIX})

