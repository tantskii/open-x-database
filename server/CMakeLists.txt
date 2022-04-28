find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.77.0 COMPONENTS system system thread log REQUIRED)

set(OMXDB_SERVER_HEADERS
    Aliases.h
    Acceptor.h
    ServerSession.h
    ServerBase.h
    Server.h)

set(OMXDB_SERVER_SOURCES
    main.cpp
    Acceptor.cpp
    ServerSession.cpp
    ServerBase.cpp
    Server.cpp)

add_executable(omxdb-server ${OMXDB_SERVER_HEADERS} ${OMXDB_SERVER_SOURCES})

target_link_libraries(omxdb-server
    PRIVATE
        ${CMAKE_PROJECT_NAME}_static
        pthread
        ${BOOST_LIBRARIES}
        Boost::log
)

target_include_directories(omxdb-server
    PRIVATE
        ${OMX_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
)

