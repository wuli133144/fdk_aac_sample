
project(decodec)
cmake_minimum_required(VERSION 2.6)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH APPEND ${PROJECT_SOURCE_DIR}/cmake/module)

include(./cmake/util.cmake)


cmake_init()

find_files(source_files)

find_libs(source_libs)

MESSAGE(STATUS "all source files:${source_files}")
MESSAGE(STATUS "all libs:${source_libs}")

add_executable(decodec ${source_files})
target_link_libraries(decodec ${source_libs})

