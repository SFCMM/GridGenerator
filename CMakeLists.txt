cmake_minimum_required(VERSION 3.16)
project(gridgenerator)
project(gridgenerator VERSION "0.0.0")

configure_file(src/config.h.in config.h @ONLY)


set(CMAKE_CXX_STANDARD 17)

#todo: move to aux/host file
include_directories(/home/svenb/build/omp411/include)
link_directories(/home/svenb/build/omp411/lib)
find_package(MPI REQUIRED)
include_directories(${MPI_CXX_INCLUDE_PATH})

include_directories(lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(gridgenerator
        src/main.cpp src/gridGenerator.cpp src/gridGenerator.h src/config.h.in src/log.cpp src/log.h src/macros.h
        src/globalmpi.h src/backtrace.h src/types.h src/sys.h src/timer.h
        src/timer.cpp src/gmath.h src/compiler_config.h src/globaltimers.h src/cartesiangrid.h)


#TARGET_LINK_LIBRARIES(gridgenerator PUBLIC mpi)
target_link_libraries(gridgenerator PUBLIC MPI::MPI_CXX)

