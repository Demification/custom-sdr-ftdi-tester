cmake_minimum_required(VERSION 3.10)
project(custom-sdr-ftdi-tester LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
)

target_link_libraries (custom-sdr-ftdi-tester
PUBLIC
    -dynamic
    -fPIC

)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")
endif()

install(TARGETS custom-sdr-ftdi-tester DESTINATION ${CMAKE_INSTALL_PREFIX})

