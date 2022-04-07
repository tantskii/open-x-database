set(OMX_HASHER_SOURCES
    IHasher.h
    NoHash.h
    CityHash128Adapter.h
    CityHash/City.h
    CityHash/CityCrc.h
)

set(OMX_HASHER_HEADERS
    HashingFactory.cpp
    NoHash.cpp
    CityHash128Adapter.cpp
    CityHash/City.cpp
)

add_library(omxdb_hashing STATIC ${OMX_HASHER_SOURCES} ${OMX_HASHER_HEADERS})
target_include_directories(omxdb_hashing PUBLIC ${OMX_INCLUDE_DIR})