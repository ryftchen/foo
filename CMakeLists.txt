# cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# project name
PROJECT(foo)

# head file path
INCLUDE_DIRECTORIES(include)

# source directory
AUX_SOURCE_DIRECTORY(source SOURCE_LIST)
AUX_SOURCE_DIRECTORY(library LIBRARY_LIST)

# set environment variable
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++-10")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++20 -stdlib=libstdc++ -W -g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++20 -stdlib=libstdc++ -W -O3")

# add executable file
ADD_EXECUTABLE(foo ${SOURCE_LIST} ${LIBRARY_LIST})

# add link library
TARGET_LINK_LIBRARIES(foo pthread)
