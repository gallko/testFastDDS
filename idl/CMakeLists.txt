cmake_minimum_required(VERSION 3.10)
project(idl_files)

file(GLOB IDL_SRC_FILES *.idl)

add_library(${PROJECT_NAME} STATIC null.cpp)

if(NOT DEFINED IDL_EXE)
    message(FATAL_ERROR "Please set IDL_EXE variable.")
endif()

# ****************************************************************
# generating
# ****************************************************************
set(IDL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(IDL_OUT_CPP ${CMAKE_CURRENT_BINARY_DIR}/out_generated_idl)
set(IDL_OUT_HPP ${CMAKE_CURRENT_BINARY_DIR}/out_generated_idl)

file(MAKE_DIRECTORY ${IDL_OUT_CPP})
file(MAKE_DIRECTORY ${IDL_OUT_HPP})

foreach(it ${IDL_SRC_FILES})
    file(RELATIVE_PATH IDL_RELATIVE_PATH ${IDL_SRC_DIR} ${it})
    get_filename_component(IDL_FILE_NAME ${IDL_RELATIVE_PATH} NAME_WE)
    get_filename_component(IDL_RELATIVE_DIRECTORY ${IDL_RELATIVE_PATH} DIRECTORY)

    string(REGEX REPLACE "^I(.*)"
            "\\1" IDL_FILE_NAME_INTERNAL
            ${IDL_FILE_NAME})

    set(IDL_OUTPUT
            ${IDL_OUT_CPP}/${IDL_RELATIVE_DIRECTORY}${IDL_FILE_NAME}.cxx
            ${IDL_OUT_HPP}/${IDL_RELATIVE_DIRECTORY}${IDL_FILE_NAME}.h
            ${IDL_OUT_CPP}/${IDL_RELATIVE_DIRECTORY}${IDL_FILE_NAME}PubSubTypes.cxx
            ${IDL_OUT_HPP}/${IDL_RELATIVE_DIRECTORY}${IDL_FILE_NAME}PubSubTypes.h)

    add_custom_command(
            OUTPUT ${IDL_OUTPUT}
            COMMENT "Generating CPP/HPP files from ${IDL_RELATIVE_PATH}"
            COMMAND ${IDL_EXE} -replace -d ${IDL_OUT_CPP} ${it}
            DEPENDS ${it}
            )

    add_custom_target(${IDL_FILE_NAME}.idl
            DEPENDS ${IDL_OUTPUT})

    add_dependencies(${PROJECT_NAME} ${IDL_FILE_NAME}.idl)

    list(APPEND IDL_SRC_FILES_SRC ${IDL_OUTPUT})
endforeach(it ${IDL_SRC_FILES})

if(IDL_SRC_FILES_SRC)
    list(APPEND IDL_SRC_FILES_SRC AvailableDataTypes.cpp)
    add_library(${PROJECT_NAME}_idl STATIC ${IDL_SRC_FILES_SRC})
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_idl)
endif()

include_directories(${IDL_OUT_HPP}/${IDL_RELATIVE_DIRECTORY})

target_include_directories(${PROJECT_NAME}_idl
        INTERFACE ${IDL_OUT_CPP}/${IDL_RELATIVE_DIRECTORY})

add_library(test::IdlGen ALIAS ${PROJECT_NAME}_idl)

target_include_directories(${PROJECT_NAME}_idl
        INTERFACE ./include)

