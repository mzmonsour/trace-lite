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
if(BACKFACE_DIAGNOSTIC EQUAL 1)
    add_definitions(-DBACKFACE_DIAGNOSTIC)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps/build)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/deps/include)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps/build/assimp)
include(ExternalProject)
ExternalProject_Add(assimp
    URL ${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp-4.1.0
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/deps/build/assimp
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/deps
)
# TODO: Cross platform link
set(libs ${libs} ${CMAKE_CURRENT_BINARY_DIR}/deps/lib/libassimp.so)

find_package(Threads REQUIRED)
set(libs ${libs} ${CMAKE_THREAD_LIBS_INIT})

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(libs ${libs} ${Boost_LIBRARIES})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
set(libs ${libs} ${PNG_LIBRARIES})

include_directories(src/)
add_executable(simple-trace ${sources})
target_link_libraries(simple-trace ${libs})

add_dependencies(simple-trace assimp)
