cmake_minimum_required(VERSION 3.15)

project("rcpc"
    VERSION 1.1.0
    LANGUAGES C
)


file(GLOB SOURCES
    *.c
)

file(GLOB HEADERS
    *.h
)

# create library
add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
