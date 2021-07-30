cmake_minimum_required(VERSION 3.16)
project(gridgenerator)
project(gridgenerator VERSION "0.0.1")

add_subdirectory(UnitTest)

configure_file(src/config.h.in config.h @ONLY)


set(CMAKE_CXX_STANDARD 17)

#todo: move to aux/host file
include_directories(/home/svenb/build/omp411/include)
link_directories(/home/svenb/build/omp411/lib)
find_package(MPI REQUIRED)
include_directories(${MPI_CXX_INCLUDE_PATH})

include_directories(lib)
include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(gridgenerator
        src/main.cpp src/gridGenerator.cpp src/gridGenerator.h src/config.h.in src/log.cpp src/log.h src/macros.h
        src/globalmpi.h src/util/backtrace.h src/types.h src/util/sys.h src/timer.h
        src/timer.cpp src/math/gmath.h src/compiler_config.h src/globaltimers.h src/cartesiangrid.h src/gtree.h
        src/gridcell.h src/WeightMethod.h src/geometry.h src/functions.h src/IO.h)


#TARGET_LINK_LIBRARIES(gridgenerator PUBLIC mpi)
target_link_libraries(gridgenerator PUBLIC MPI::MPI_CXX)

