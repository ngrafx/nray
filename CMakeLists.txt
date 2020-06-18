# usage
# cmake -DCMAKE_BUILD_TYPE=Debug
# cmake -DCMAKE_BUILD_TYPE=Release

cmake_minimum_required(VERSION 3.6)

add_definitions(-std=c++17)

# set(CXX_FLAGS "-Wall -O4")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")


project(nray)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES
    "src/*.cpp"
    "src/*.h"
)

include_directories("external/")

add_executable(nray ${SOURCES})
# Seems that Linux cannot always find pthread
target_link_libraries( nray pthread )