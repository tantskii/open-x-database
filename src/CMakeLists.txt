
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.77.0 COMPONENTS system thread REQUIRED)

set(OMX_HEADERS_DIR ${OMX_INCLUDE_DIR}/omx)
set(OMX_PUBLIC_HEADERS
    ${OMX_HEADERS_DIR}/Database.h
    ${OMX_HEADERS_DIR}/Key.h
    ${OMX_HEADERS_DIR}/Options.h
    ${OMX_HEADERS_DIR}/internal/FastPimpl.h
    ${OMX_HEADERS_DIR}/internal/Export.h
)

set(OMX_SOURCES
    ${OMX_SOURCE_DIR}/Database.cpp
    ${OMX_SOURCE_DIR}/Key.cpp
    ${OMX_SOURCE_DIR}/MemTable.cpp
    ${OMX_SOURCE_DIR}/SSTableRow.cpp
    ${OMX_SOURCE_DIR}/Index.cpp
    ${OMX_SOURCE_DIR}/WriteAheadLog.cpp
    ${OMX_SOURCE_DIR}/StorageEngine.cpp
    ${OMX_SOURCE_DIR}/SSTable.cpp
    ${OMX_SOURCE_DIR}/File.cpp
)
set(OMX_HEADERS
    ${OMX_SOURCE_DIR}/MemTable.h
    ${OMX_SOURCE_DIR}/MemTableKey.h
    ${OMX_SOURCE_DIR}/SSTableRow.h
    ${OMX_SOURCE_DIR}/Index.h
    ${OMX_SOURCE_DIR}/WriteAheadLog.h
    ${OMX_SOURCE_DIR}/StorageEngine.h
    ${OMX_SOURCE_DIR}/SSTable.h
    ${OMX_SOURCE_DIR}/File.h
)

find_package(Threads REQUIRED)

set(OMX_COMPILE_OPTIONS -Wall -Wextra -pedantic -Werror -fno-rtti)

add_subdirectory(Compression)
add_subdirectory(Hasher)

macro(build_omxdb_lib lib_name lib_type)
    add_library(${lib_name}
        ${lib_type}
            ${OMX_SOURCES}
            ${OMX_HEADERS}
            ${OMX_PUBLIC_HEADERS}
    )

    target_include_directories(${lib_name}
        PUBLIC
            ${OMX_INCLUDE_DIR}
            ${Boost_INCLUDE_DIR}
    )

    target_link_libraries(${lib_name}
        PUBLIC
            omxdb_compression
            omxdb_hashing
            ${BOOST_LIBRARIES} Boost::system
    )

    set_target_properties(${lib_name}
        PROPERTIES
        DEFINE_SYMBOL "OMXDB_DLL_EXPORTS"
        OUTPUT_NAME   ${CMAKE_PROJECT_NAME}
        VERSION       ${OMX_VERSION_MAJOR}.${OMX_VERSION_MINOR}
        SOVERSION     ${OMX_VERSION_MAJOR}
    )
endmacro()

build_omxdb_lib(${CMAKE_PROJECT_NAME} SHARED)
if (OMX_WITH_UNITTESTS)
    build_omxdb_lib(${CMAKE_PROJECT_NAME}_static STATIC)
endif()

install(
    DIRECTORY ${OMX_HEADERS_DIR}
    DESTINATION ${OMX_INSTALL_PREFIX}/include
    COMPONENT headers)

install(TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION ${OMX_INSTALL_PREFIX}/bin/${OMX_OUTPUT_PREFIX}
    ARCHIVE DESTINATION ${OMX_INSTALL_PREFIX}/lib/${OMX_OUTPUT_PREFIX}
    LIBRARY DESTINATION ${OMX_INSTALL_PREFIX}/lib/${OMX_OUTPUT_PREFIX})

install(
    FILES ${CMAKE_SOURCE_DIR}/cmake/Findomxdb.cmake
    DESTINATION ${OMX_INSTALL_PREFIX}/cmake
    COMPONENT cmake)
