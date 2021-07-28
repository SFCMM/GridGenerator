project(UnitTest)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}  ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/src)

# openmpi
include_directories(/home/svenb/build/omp411/include)
link_directories(/home/svenb/build/omp411/lib)

# adding the Google_Tests_run target
add_executable(UnitTest test_hilbert.cpp)
target_link_libraries(UnitTest gtest gtest_main)

target_compile_options(UnitTest PUBLIC --std=c++17)