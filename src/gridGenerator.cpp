#include <mpi.h>

#include "gridGenerator.h"

#ifdef _OPENMP
#include <omp.h>
#endif


template <int DEBUG_LEVEL>
auto GridGenerator<DEBUG_LEVEL>::run(int argc, char** argv) -> int {
#ifdef _OPENMP
  int provided = 0;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
#else
  MPI_Init(&argc, &argv);
#endif

  return 0;
}