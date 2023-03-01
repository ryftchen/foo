cmake_minimum_required(VERSION 3.0)

project(foo_test)

include(CheckCXXCompilerFlag)
include(CheckIncludeFile)
check_cxx_compiler_flag(-std=c++20 COMPILER_SUPPORTS_CXX20)
check_include_file("gtest/gtest.h" HAVE_GTEST_HEADER)
check_include_file("mpfr.h" HAVE_MPFR_HEADER)
find_package(GTest REQUIRED)

if(DEFINED ENV{FOO_ENV})
    message(STATUS "The test project env: FOO_ENV=$ENV{FOO_ENV}")
endif()
include(${CMAKE_SOURCE_DIR}/../cmake/common.cmake)
include(${CMAKE_SOURCE_DIR}/../cmake/toolchain.cmake)
set(RUNTIME_PRINTING OFF CACHE BOOL "runtime printing" FORCE)
include(${CMAKE_SOURCE_DIR}/../cmake/condition.cmake)

set(BIN_NAME foo_test)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)

file(GLOB TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/unit TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../algorithm/source TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../data_structure/source TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../design_pattern/source TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../numeric/source TEST_LIST)
include_directories(
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../algorithm
    ${CMAKE_SOURCE_DIR}/../algorithm/include
    ${CMAKE_SOURCE_DIR}/../data_structure
    ${CMAKE_SOURCE_DIR}/../data_structure/include
    ${CMAKE_SOURCE_DIR}/../design_pattern
    ${CMAKE_SOURCE_DIR}/../design_pattern/include
    ${CMAKE_SOURCE_DIR}/../numeric
    ${CMAKE_SOURCE_DIR}/../numeric/include
)

add_executable(${BIN_NAME} ${TEST_LIST})
target_link_libraries(
    ${BIN_NAME}
    PUBLIC
        mpfr
        GTest::Main
)
if(PRECOMPILED_HEADER)
    target_precompile_headers(
        ${BIN_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/../algorithm/pch_algorithm.hpp
            ${CMAKE_SOURCE_DIR}/../data_structure/pch_data_structure.hpp
            ${CMAKE_SOURCE_DIR}/../design_pattern/pch_design_pattern.hpp
            ${CMAKE_SOURCE_DIR}/../numeric/pch_numeric.hpp
    )
endif()
