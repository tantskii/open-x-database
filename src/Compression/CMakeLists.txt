set(OMX_COMPRESSION_SOURCES
    CompressionFactory.cpp
    NoCompression.cpp
    SnappyCompressionAdapter.cpp
)

set(OMX_COMPRESSION_HEADERS
    ICompression.h
    CompressionType.h
    NoCompression.h
    SnappyCompressionAdapter.h
)

add_library(omxdb_compression STATIC ${OMX_COMPRESSION_SOURCES} ${OMX_COMPRESSION_HEADERS})
target_link_libraries(omxdb_compression PUBLIC snappy)