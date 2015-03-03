cmake_minimum_required (VERSION 2.8)
project (util)

set(LIBRARY_NAME util)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(
    GLOB_RECURSE
    SRCS
    src/*
)

add_library(${LIBRARY_NAME} STATIC ${SRCS})