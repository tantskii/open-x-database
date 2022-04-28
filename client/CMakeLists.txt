find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.77.0 COMPONENTS system system thread log REQUIRED)

set(OMXDB_CLIENT_HEADERS
    Aliases.h
    DatabaseConnection.h
    ClientSession.h
)

set(OMXDB_CLIENT_SOURCES
    main.cpp
    DatabaseConnection.cpp
    ClientSession.cpp
)

add_executable(omxdb-client ${OMXDB_CLIENT_HEADERS} ${OMXDB_CLIENT_SOURCES})

target_link_libraries(omxdb-client
    PRIVATE
        ${CMAKE_PROJECT_NAME}_static
        pthread
        ${BOOST_LIBRARIES}
        Boost::log
)

target_include_directories(omxdb-client
    PRIVATE
        ${OMX_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
)