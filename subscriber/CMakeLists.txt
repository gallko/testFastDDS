cmake_minimum_required(VERSION 3.10)
project(testSubscriber)

find_package(Threads)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.cpp)

include_directories(../publisher/include)
include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Threads::Threads test::IdlGen test::Utils fastcdr fastrtps nlohmann_json::nlohmann_json)