cmake_minimum_required(VERSION 3.17)
project(omxdb)

set(CMAKE_CXX_STANDARD 20)

option(OMX_WITH_UNITTESTS "Build unittests exectable" ON)

set(OMX_VERSION_MAJOR 1)
set(OMX_VERSION_MINOR 0)
set(OMX_VERSION_RELEASE 0)

set(OMX_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(extern/leveldb)
add_subdirectory(src)

if (OMX_WITH_UNITTESTS)
    add_subdirectory(test)
endif()


