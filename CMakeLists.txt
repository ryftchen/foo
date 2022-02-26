#1.cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

#2.project name
PROJECT(foo)

#3.head file path
INCLUDE_DIRECTORIES(include)

#4.source directory
AUX_SOURCE_DIRECTORY(source SOURCE_LIST)
AUX_SOURCE_DIRECTORY(library LIBRARY_LIST)

#5.set environment variable
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++20 -stdlib=libstdc++ -W -g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++20 -stdlib=libstdc++ -W -O3")

#6.add executable file
ADD_EXECUTABLE(foo ${SOURCE_LIST} ${LIBRARY_LIST})
#ADD_LIBRARY(common SHARED ${LIBRARY_LIST})
#ADD_EXECUTABLE(foo ${SOURCE_LIST})

#7.add link library
TARGET_LINK_LIBRARIES(foo pthread)
#TARGET_LINK_LIBRARIES(foo common)
