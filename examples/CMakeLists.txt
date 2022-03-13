cmake_minimum_required(VERSION 3.17)
project(example_omxdb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(omxdb)
find_package(Threads REQUIRED)

add_executable(example_omxdb main.cpp)
target_include_directories(example_omxdb PRIVATE ${OMXDB_INCLUDE_DIRS})
target_link_libraries(example_omxdb PRIVATE ${OMXDB_LIBRARIES} ${OMXDB_RUNTIME_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
