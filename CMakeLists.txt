cmake_minimum_required(VERSION 3.0)

project(foo)

include(CheckCXXCompilerFlag)
include(CheckIncludeFile)
include(CheckIncludeFiles)
check_cxx_compiler_flag(-std=c++20 COMPILER_SUPPORTS_CXX20)
check_include_file("bits/stdint-uintn.h" HAVE_BITS_HEADER)
check_include_file("ext/stdio_filebuf.h" HAVE_EXT_HEADER)
check_include_file("cxxabi.h" HAVE_CXXABI_HEADER)
check_include_file("dlfcn.h" HAVE_DLFCN_HEADER)
check_include_file("execinfo.h" HAVE_EXECINFO_HEADER)
check_include_file("mpfr.h" HAVE_MPFR_HEADER)
check_include_file("unistd.h" HAVE_UNISTD_HEADER)
check_include_files("readline/history.h;readline/readline.h" HAVE_READLINE_HEADER)
check_include_files("sys/file.h;sys/time.h" HAVE_SYS_HEADER)
find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES Off)
set(CMAKE_C_COMPILER /usr/bin/clang-12)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-12)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
if(DEFINED ENV{FOO_ENV})
    message(STATUS "FOO_ENV=$ENV{FOO_ENV}")
    if($ENV{FOO_ENV} STREQUAL "CODE_COVERAGE")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    endif()
endif()
add_compile_options(-std=c++20 -stdlib=libstdc++ -W -fcolor-diagnostics)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-export-dynamic")
endif()
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time") 
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")

set(BIN_NAME foo)
set(BUILD_DIR ${PROJECT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/lib)

aux_source_directory(${CMAKE_SOURCE_DIR}/application/source APPLICATION_LIST)
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(utility)
add_subdirectory(algorithm)
add_subdirectory(design_pattern)
add_subdirectory(numeric)

add_executable(${BIN_NAME} ${APPLICATION_LIST})
target_include_directories(
    ${BIN_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/application/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/application/source)
target_link_libraries(
    ${BIN_NAME}
    PUBLIC
        dl
        pthread
        readline
        mpfr
    PRIVATE
        foo::utility
        foo::algorithm
        foo::design_pattern
        foo::numeric)
