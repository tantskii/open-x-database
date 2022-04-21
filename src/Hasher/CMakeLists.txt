set(CMAKE_CXX_STANDARD 14)

set(OMX_HASHER_SOURCES
    IHasher.h
    NoHash.h
    CityHash128Adapter.h
    MurmurHash3Adapter.h
)

set(OMX_HASHER_HEADERS
    HashingFactory.cpp
    NoHash.cpp
    CityHash128Adapter.cpp
    MurmurHash3Adapter.cpp
)

add_subdirectory(SMHasher/src)

add_library(omxdb_hashing STATIC ${OMX_HASHER_SOURCES} ${OMX_HASHER_HEADERS})
target_link_libraries(omxdb_hashing PUBLIC SMHasherSupport)
target_include_directories(omxdb_hashing PUBLIC ${OMX_INCLUDE_DIR} SMHasher/src)