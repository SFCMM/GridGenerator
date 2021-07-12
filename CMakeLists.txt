cmake_minimum_required(VERSION 3.19)
project(gridgenerator)

set(CMAKE_CXX_STANDARD 17)

include_directories(lib)

add_executable(gridgenerator
        src/main.cpp src/gridGenerator.cpp src/gridGenerator.h)
