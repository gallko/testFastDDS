cmake_minimum_required(VERSION 3.10)
project(utils)

find_package(Threads)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(test::Utils ALIAS ${PROJECT_NAME})