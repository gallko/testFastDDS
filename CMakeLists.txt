cmake_minimum_required(VERSION 3.10)
project(testFastDDS)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(NOT fastcdr_FOUND)
    find_package(fastcdr REQUIRED)
endif()
if(NOT fastrtps_FOUND)
    find_package(fastrtps REQUIRED)
endif()

add_subdirectory(idl)
add_subdirectory(utils)
add_subdirectory(publisher)
add_subdirectory(subscriber)
