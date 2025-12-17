cmake_minimum_required(VERSION 3.20)

project(foo_test)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++20 COMPILER_SUPPORTS_CXX20)

find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSL REQUIRED gsl)
pkg_check_modules(MPFR REQUIRED mpfr)

if(DEFINED ENV{FOO_ENV})
    message(STATUS "The test project env: FOO_ENV=$ENV{FOO_ENV}")
endif()
include(${CMAKE_SOURCE_DIR}/../cmake/common.cmake)
if((DEFINED ENV{FOO_BLD_COV}) AND ("$ENV{FOO_BLD_COV}" STREQUAL "llvm-cov"))
    message(STATUS "Checking coverage")
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
endif()
if(DEFINED ENV{FOO_BLD_SAN})
    if("$ENV{FOO_BLD_SAN}" STREQUAL "asan")
        set(SANITIZER_TYPE "address")
    elseif("$ENV{FOO_BLD_SAN}" STREQUAL "tsan")
        set(SANITIZER_TYPE "thread")
    elseif("$ENV{FOO_BLD_SAN}" STREQUAL "ubsan")
        set(SANITIZER_TYPE "undefined")
    endif()
    if(SANITIZER_TYPE)
        message(STATUS "Enabling -fsanitize=${SANITIZER_TYPE}")
        add_compile_options(-Werror -fsanitize=${SANITIZER_TYPE})
        add_link_options(-fsanitize=${SANITIZER_TYPE})
    endif()
endif()
include(${CMAKE_SOURCE_DIR}/../cmake/toolchain.cmake)
set(RUNTIME_PRINTING OFF CACHE BOOL "runtime printing" FORCE)
include(${CMAKE_SOURCE_DIR}/../cmake/condition.cmake)

set(BIN_NAME foo_test)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/lib)
include(${CMAKE_SOURCE_DIR}/ut_module.cmake)

aux_source_directory(${CMAKE_SOURCE_DIR}/unit TEST_LIST)
include_directories(
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../algorithm/include
    ${CMAKE_SOURCE_DIR}/../design_pattern/include
    ${CMAKE_SOURCE_DIR}/../data_structure/include
    ${CMAKE_SOURCE_DIR}/../numeric/include)

add_executable(${BIN_NAME} ${TEST_LIST})
foreach(MODULE_NAME IN LISTS MODULE_TARGETS)
    target_link_libraries(${BIN_NAME} PRIVATE ${PROJECT_NAME}::${MODULE_NAME})
endforeach()
target_link_libraries(${BIN_NAME} PRIVATE GTest::gtest ${GSL_LIBRARIES} ${MPFR_LIBRARIES})

if(PRECOMPILED_HEADER)
    target_precompile_headers(${BIN_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../application/pch/precompiled_header.hpp)
endif()
