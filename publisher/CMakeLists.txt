cmake_minimum_required(VERSION 3.10)
project(testPublisher)

find_package(Threads)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

include_directories(include)
include_directories(../ext_lib/parser_args)

add_executable(${PROJECT_NAME} ${SOURCES})
message(STATUS fastcdr)

target_link_libraries(${PROJECT_NAME} Threads::Threads test::IdlGen test::Utils fastcdr fastrtps parser_args)