project(simple-trace)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(sources
    src/main.cpp
    src/obj_file.cpp
    src/model.cpp
    src/render.cpp
    src/trace.cpp
    src/png_helper.c
    )

if(USE_DOUBLE_PRECISION EQUAL 1)
    add_definitions(-DUSE_DOUBLE_PRECISION)
endif()

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(libs ${libs} ${Boost_LIBRARIES})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
set(libs ${libs} ${PNG_LIBRARIES})

include_directories(src/)
add_executable(simple-trace ${sources})
target_link_libraries(simple-trace ${libs})
