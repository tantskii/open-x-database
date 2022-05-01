find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

macro(add_test test_name sources)
    set(binary ${test_name})

    add_executable(${binary}
        GoogleTestDriver.cpp
        ${sources}
    )

    target_link_libraries(${binary}
        PRIVATE
            ${CMAKE_PROJECT_NAME}_static
            gtest
            pthread
    )

    target_include_directories(${binary}
        PRIVATE
            ${OMX_INCLUDE_DIR}
            ${OMX_SOURCE_DIR}
    )
endmacro()

add_test(omxdb-test Test.cpp)

install(TARGETS omxdb-test RUNTIME DESTINATION ${OMX_INSTALL_PREFIX}/bin/${OMX_OUTPUT_PREFIX})
