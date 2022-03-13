# Find the omx database library.
# Sets the usual variables expected for find_package scripts:
# OMXDB_INCLUDE_DIR - headers location
# OMXDB_LIBRARIES - libraries to link against
# OMXDB_RUNTIME_LIBRARIES - runtime libraries
# OMXDB_FOUND - true if omxdb was found.

# This is the directory where the omxdb is located.
# By default OMXDBDIR environment variable value is taken.
set(OMXDB_ROOT "$ENV{OMXDBDIR}" CACHE PATH "omxdb root directory.")

# Look for headers.
find_path(OMXDB_INCLUDE_DIRS
    NAMES omx/BinaryStorage.h omx/Bytes.h omx/Key.h omx/Options.h omx/Transaction.h
    HINTS $ENV{OMXDBDIR}
    PATHS ${OMXDB_ROOT}
    PATH_SUFFIXES include)

if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
elseif(APPLE AND NOT IOS) # mac os
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".dylib")
else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
endif()

set(OMXDB_LIB_PREFIX "")
set(OMXDB_LIB_NAMES omxdb)
set(OMXDB_LIB_PATH_SUFFIX lib/${OMXDB_LIB_PREFIX})
set(OMXDB_BIN_PATH_SUFFIX bin/${OMXDB_LIB_PREFIX})

macro(find_omxdb_library LIBRARY_VAR LIBRARY_NAME)
    find_library(${LIBRARY_VAR}
        NAMES ${LIBRARY_NAME}
        HINTS $ENV{OMXDBDIR}
        PATHS ${OMXDB_ROOT}
        PATH_SUFFIXES ${OMXDB_LIB_PATH_SUFFIX}
        NO_DEFAULT_PATH
    )
endmacro()

find_omxdb_library(OMXDB_LIBRARY_RELEASE omxdb)
find_omxdb_library(OMXDB_LIBRARY_DEBUG omxdbd)

if (WIN32)
    macro(find_omxdb_runtime_library LIBRARY_VAR LIBRARY_NAME)
        find_library(${LIBRARY_VAR}
            NAMES ${LIBRARY_NAME}
            HINTS $ENV{OMXDBDIR}
            PATHS ${OMXDB_ROOT}
            PATH_SUFFIXES ${OMXDB_BIN_PATH_SUFFIX}
            NO_DEFAULT_PATH
        )
    endmacro()

    find_omxdb_runtime_library(OMXDB_RUNTIME_LIBRARY_RELEASE omxdb)
    find_omxdb_runtime_library(OMXDB_RUNTIME_LIBRARY_DEBUG omxdbd)
endif()

# link components
if(OMXDB_LIBRARY_RELEASE AND OMXDB_LIBRARY_DEBUG)
    set(OMXDB_LIBRARIES optimized ${OMXDB_LIBRARY_RELEASE} debug ${OMXDB_LIBRARY_DEBUG})
elseif(OMXDB_LIBRARY_DEBUG)
    set(OMXDB_LIBRARIES ${OMXDB_LIBRARY_DEBUG})
    message(STATUS "[OMXDB]: Release libraries were not found")
elseif(OMXDB_LIBRARY_RELEASE)
    set(OMXDB_LIBRARIES ${OMXDB_LIBRARY_RELEASE})
    message(STATUS "[OMXDB]: Debug libraries were not found")
endif()

# runtime components
if(WIN32)
    if(OMXDB_RUNTIME_LIBRARY_RELEASE)
        list(APPEND OMXDB_RUNTIME_LIBRARIES ${OMXDB_RUNTIME_LIBRARY_RELEASE})
    endif()

    if(OMXDB_RUNTIME_LIBRARY_DEBUG)
        list(APPEND OMXDB_RUNTIME_LIBRARIES ${OMXDB_RUNTIME_LIBRARY_DEBUG})
    endif()
else()
    list(APPEND OMXDB_RUNTIME_LIBRARIES ${OMXDB_LIBRARIES})
endif()

message(STATUS "[OMXDB]: OMXDB_INCLUDE_DIRS: ${OMXDB_INCLUDE_DIRS}")
message(STATUS "[OMXDB]: OMXDB_LIBRARIES: ${OMXDB_LIBRARIES}")
message(STATUS "[OMXDB]: OMXDB_RUNTIME_LIBRARIES: ${OMXDB_RUNTIME_LIBRARIES}")

# Support the REQUIRED and QUIET arguments, and set MATCHINGKERNEL_FOUND if found.
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(omxdb DEFAULT_MSG
    OMXDB_INCLUDE_DIRS
    OMXDB_LIBRARIES
    OMXDB_RUNTIME_LIBRARIES
)

# Don't show in GUI
mark_as_advanced(
    OMXDB_INCLUDE_DIRS
    OMXDB_LIBRARIES
    OMXDB_LIB_PREFIX
    OMXDB_LIB_NAMES
)