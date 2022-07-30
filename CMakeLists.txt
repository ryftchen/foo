cmake_minimum_required(VERSION 3.0)

project(foo)

set(BIN_NAME foo)
set(BUILD_DIR ${PROJECT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-12")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
if(DEFINED ENV{FOO_ENV})
    message(STATUS "The environment variable FOO_ENV is $ENV{FOO_ENV}")
    if($ENV{FOO_ENV} STREQUAL "CODE_COVERAGE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    endif()
endif()
add_compile_options(-std=c++20 -stdlib=libstdc++ -W)

aux_source_directory(application/source APPLICATION_LIST)
include_directories(utility/include algorithm/include)

add_subdirectory(utility)
add_subdirectory(algorithm)

add_executable(${BIN_NAME} ${APPLICATION_LIST})
target_include_directories(${BIN_NAME} PUBLIC application/include PRIVATE application/source)
target_link_libraries(${BIN_NAME} utility algorithm pthread)
