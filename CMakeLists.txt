cmake_minimum_required(VERSION 3.3)
project(major_assignment_1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -lpthread -Wall")

set(SOURCE_FILES
    src/main.c)

include_directories(src)

add_executable(shell ${SOURCE_FILES})