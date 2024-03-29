cmake_minimum_required(VERSION 3.16)
project(gridgenerator)
project(gridgenerator VERSION "0.0.3")

add_subdirectory(UnitTest)
add_subdirectory(perf)

configure_file(src/config.h.in config.h @ONLY)


set(CMAKE_CXX_STANDARD 17)

#todo: move to presets file
#include_directories(/home/svenb/build/omp411/include)
#link_directories(/home/svenb/build/omp411/lib)

find_package(MPI REQUIRED)
include_directories(${MPI_CXX_INCLUDE_PATH})

include_directories(SYSTEM external)
include_directories(src include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(gridgenerator
        src/main.cpp src/gridGenerator.cpp src/gridGenerator.h src/config.h.in
        src/globaltimers.h src/cartesiangrid.h
        src/gridcell_properties.h src/loadbalancing_weights.h src/geometry.h src/functions.h src/common/IO.h src/common/term.h
        src/cartesiangrid_generation.h src/cartesiangrid_base.h src/common/line.h src/common/mesh.h)

#TARGET_LINK_LIBRARIES(gridgenerator PUBLIC mpi)
target_link_libraries(gridgenerator PUBLIC MPI::MPI_CXX)

option(BUILD_DOC "Build documentation" OFF)
# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

