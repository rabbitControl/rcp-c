cmake_minimum_required(VERSION 3.15)

project("rcpc" LANGUAGES C VERSION 1.1.0)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


file(GLOB SOURCES
    *.c
)

file(GLOB HEADERS
    *.h
)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
