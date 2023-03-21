# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# project name and language
project(o7p LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

set(BUILD_DIR "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/../bin")

set(BINARY_DIR "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BINARY_DIR}/../lib")

add_subdirectory(src/compiler)
add_subdirectory(src/launcher)
add_subdirectory(src/smodel)
add_subdirectory(src/object_model)
