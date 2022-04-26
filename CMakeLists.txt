cmake_minimum_required(VERSION 3.17)
project(omxdb)

set(CMAKE_CXX_STANDARD 20)

option(OMX_WITH_UNITTESTS "Build unittests exectable" ON)
option(OMX_WITH_SERVER "Build omxdb-server executable" ON)
option(OMX_WITH_CLIENT "Build omxdb-client executable" ON)

set(OMX_VERSION_MAJOR 1)
set(OMX_VERSION_MINOR 0)
set(OMX_VERSION_RELEASE 0)

set(OMX_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(OMX_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(OMX_OUTPUT_PREFIX "")
set(OMX_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

add_subdirectory(src)
add_subdirectory(extern/snappy)
include_directories(extern/snappy)
add_subdirectory(extern/lz4/build/cmake)
include_directories(extern/lz4/lib)

if (OMX_WITH_UNITTESTS)
    add_subdirectory(extern/googletest)
    add_subdirectory(test)
endif()

if (OMX_WITH_SERVER)
    add_subdirectory(server)
endif()

if (OMX_WITH_CLIENT)
    add_subdirectory(client)
endif()
