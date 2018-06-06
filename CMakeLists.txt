project(simple-trace)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(sources
    src/main.cpp
    src/obj_file.cpp
    src/model.cpp
    )

include_directories(src/)
add_executable(simple-trace ${sources})
