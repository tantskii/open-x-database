find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

macro(add_test test_name sources)
    set(binary unittest_${test_name})

    add_executable(${binary}
        GoogleTestDriver.cpp
        ${sources}
    )

    target_link_libraries(${binary}
        PRIVATE
            ${CMAKE_PROJECT_NAME}
            gtest
            pthread
    )

    target_include_directories(${binary}
        PRIVATE
            ${OMX_INCLUDE_DIR}
    )
endmacro()

add_test(omxdb Test.cpp)

install(TARGETS unittest_omxdb RUNTIME DESTINATION ${OMX_INSTALL_PREFIX}/bin/${OMX_OUTPUT_PREFIX})
