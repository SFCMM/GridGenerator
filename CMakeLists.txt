cmake_minimum_required(VERSION 3.16)
project(gridgenerator)
project(gridgenerator VERSION "0.0.1")

add_subdirectory(UnitTest)
add_subdirectory(perf)

configure_file(src/config.h.in config.h @ONLY)


set(CMAKE_CXX_STANDARD 17)

#todo: move to presets file
include_directories(/home/svenb/build/omp411/include)
link_directories(/home/svenb/build/omp411/lib)
find_package(MPI REQUIRED)
include_directories(${MPI_CXX_INCLUDE_PATH})

include_directories(external)
include_directories(src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(gridgenerator
        src/main.cpp src/gridGenerator.cpp src/gridGenerator.h src/config.h.in src/log.cpp src/log.h src/macros.h
        src/common/globalmpi.h src/util/backtrace.h src/common/types.h src/util/sys.h src/timer.h
        src/timer.cpp src/math/mathfunctions.h src/common/compiler_config.h src/globaltimers.h src/cartesiangrid.h src/gtree.h
        src/gridcell_properties.h src/WeightMethod.h src/geometry.h src/functions.h src/IO.h src/cartesian.h src/util/string_helper.h)


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

