cmake_minimum_required(VERSION 3.19)
project(gridgenerator)
project(gridgenerator VERSION "0.0.0")

configure_file(src/config.h.in config.h @ONLY)


set(CMAKE_CXX_STANDARD 17)

include_directories(lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(gridgenerator
        src/main.cpp src/gridGenerator.cpp src/gridGenerator.h src/config.h.in src/log.cpp src/log.h src/macros.h src/globalmpi.h src/globalmpi.cpp src/backtrace.h src/global.cpp src/global.h src/types.h)


#todo: move to aux/host file
include_directories(/home/svenb/build/omp411/include)
link_directories(/home/svenb/build/omp411/lib)
TARGET_LINK_LIBRARIES(gridgenerator mpi)
