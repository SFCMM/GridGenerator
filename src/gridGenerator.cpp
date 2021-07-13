#include <iostream>
#include <mpi.h>

#include "gridGenerator.h"

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace GRIDGEN;

std::ostream cerr0(nullptr);

template <int DEBUG_LEVEL>
auto GridGenerator<DEBUG_LEVEL>::run(int argc, char** argv) -> int {
#ifdef _OPENMP
  int provided = 0;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
#else
  MPI_Init(&argc, &argv);
#endif

  MPI_Comm_rank(MPI_COMM_WORLD, &m_domainId);
  MPI_Comm_size(MPI_COMM_WORLD, &m_noDomains);

  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

  // Open cerr0 on MPI root
  if(m_domainId == 0) {
    cerr0.rdbuf(std::cerr.rdbuf());
  } else {
    cerr0.rdbuf(&nullBuffer);
  }

  return 0;
}

template class GRIDGEN::GridGenerator<0>;
template class GRIDGEN::GridGenerator<1>;
template class GRIDGEN::GridGenerator<2>;
template class GRIDGEN::GridGenerator<3>;
template class GRIDGEN::GridGenerator<4>;