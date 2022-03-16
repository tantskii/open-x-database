
set(OMX_HEADERS_DIR ${OMX_INCLUDE_DIR}/omx)
set(OMX_PUBLIC_HEADERS
    ${OMX_HEADERS_DIR}/BinaryStorage.h
    ${OMX_HEADERS_DIR}/Bytes.h
    ${OMX_HEADERS_DIR}/Key.h
    ${OMX_HEADERS_DIR}/Options.h
    ${OMX_HEADERS_DIR}/Transaction.h
    ${OMX_HEADERS_DIR}/internal/FastPimpl.h
    ${OMX_HEADERS_DIR}/internal/Export.h
)

set(OMX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(OMX_SOURCES
    ${OMX_SOURCE_DIR}/BinaryStorage.cpp
    ${OMX_SOURCE_DIR}/Bytes.cpp
    ${OMX_SOURCE_DIR}/Key.cpp
    ${OMX_SOURCE_DIR}/Options.cpp
    ${OMX_SOURCE_DIR}/Transaction.cpp
    ${OMX_SOURCE_DIR}/OptionsImpl.cpp
    ${OMX_SOURCE_DIR}/BinaryStorageImpl.cpp
    ${OMX_SOURCE_DIR}/TransactionImpl.cpp
    ${OMX_SOURCE_DIR}/KeyComparator.cpp
)
set(OMX_HEADERS
    ${OMX_SOURCE_DIR}/OptionsImpl.h
    ${OMX_SOURCE_DIR}/BinaryStorageImpl.h
    ${OMX_SOURCE_DIR}/TransactionImpl.h
    ${OMX_SOURCE_DIR}/KeyComparator.h
)

set(OMX_COMPILER_NAME ${CMAKE_CXX_COMPILER_ID})
set(OMX_TARGET_NAME ${CMAKE_SYSTEM_PROCESSOR})

# set(OMX_OUTPUT_PREFIX ${OMX_COMPILER_NAME}/${OMX_TARGET_NAME})
set(OMX_OUTPUT_PREFIX "")
set(OMX_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

find_package(Threads REQUIRED)

set(OMX_COMPILE_OPTIONS -Wall -Wextra -pedantic -Werror -fno-rtti)

add_library(${CMAKE_PROJECT_NAME}
    SHARED
        ${OMX_SOURCES}
        ${OMX_HEADERS}
        ${OMX_PUBLIC_HEADERS}
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${LEVELDB_PUBLIC_INCLUDE_DIR}
        ${OMX_INCLUDE_DIR}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        leveldb
)

target_compile_options(${CMAKE_PROJECT_NAME}
    PUBLIC
        ${OMX_COMPILE_OPTIONS}
)

set_target_properties(${CMAKE_PROJECT_NAME}
    PROPERTIES
        DEFINE_SYMBOL "OMXDB_DLL_EXPORTS"
        OUTPUT_NAME   ${CMAKE_PROJECT_NAME}
        VERSION       ${OMX_VERSION_MAJOR}.${OMX_VERSION_MINOR}
        SOVERSION     ${OMX_VERSION_MAJOR}
)

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
