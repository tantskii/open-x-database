set(OMX_COMPRESSION_SOURCES
    CompressionFactory.cpp
    NoCompression.cpp
    SnappyCompressionAdapter.cpp
    LZ4CompressionAdapter.cpp
)

set(OMX_COMPRESSION_HEADERS
    ICompression.h
    NoCompression.h
    SnappyCompressionAdapter.h
    LZ4CompressionAdapter.h
)

add_library(omxdb_compression STATIC ${OMX_COMPRESSION_SOURCES} ${OMX_COMPRESSION_HEADERS})
target_link_libraries(omxdb_compression PUBLIC snappy lz4_static)
target_include_directories(omxdb_compression PUBLIC ${OMX_INCLUDE_DIR})