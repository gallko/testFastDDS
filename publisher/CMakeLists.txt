cmake_minimum_required(VERSION 3.10)
project(testPublisher)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME})