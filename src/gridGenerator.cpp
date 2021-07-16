#include <iostream>
#include <mpi.h>

#include "config.h.in"
#include "constants.h"
#include "globalmpi.h"
#include "gridGenerator.h"
#include "sys.h"
#include "timer.h"
//#include "macros.h"

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace GRIDGEN;
using namespace std;

std::ostream cerr0(nullptr); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

template <GInt DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::init(int argc, GChar** argv) {
#ifdef _OPENMP
  int provided = 0;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
#else
  MPI_Init(&argc, &argv);
#endif

  MPI_Comm_rank(MPI_COMM_WORLD, &m_domainId);
  MPI_Comm_size(MPI_COMM_WORLD, &m_noDomains);
  MPI::g_mpiInformation.init(m_domainId, m_noDomains);

  MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

  // Open cerr0 on MPI root
  if(m_domainId == 0) {
    cerr0.rdbuf(std::cerr.rdbuf());
  } else {
    cerr0.rdbuf(&nullBuffer);
  }

#ifndef GRIDGEN_SINGLE_FILE_LOG
  gridgen_log.open("gridgen_log" + std::to_string(m_domainId), false, argc, argv, MPI_COMM_WORLD);
#else
  if(DEBUG_LEVEL < MORE_DEBUG) {
    gridgen_log.open("gridgen_log", true, argc, argv, MPI_COMM_WORLD);
  } else {
    gridgen_log.open("gridgen_log", false, argc, argv, MPI_COMM_WORLD);
  }
#endif
  gridgen_log.setMinFlushSize(LOG_MIN_FLUSH_SIZE);

  RESET_TIMERS();
}

template <GInt DEBUG_LEVEL>
auto GridGenerator<DEBUG_LEVEL>::run() -> int {
  gridgen_log << "Grid generator started"<<endl;
  showLogo();
  loadConfiguration();
  generateGrid();
  gridgen_log << "Grid generator finished"<<endl;

  return 0;
}
template <GInt DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::showLogo() {
  cout <<  R"(    __  _______  __  _________     _     __)"<< endl;
  cout <<  R"(   /  |/  / __ \/  |/  / ____/____(_)___/ /)"<< endl;
  cout <<  R"(  / /|_/ / / / / /|_/ / / __/ ___/ / __  / )"<< endl;
  cout <<  R"( / /  / / /_/ / /  / / /_/ / /  / / /_/ /  )"<< endl;
  cout <<  R"(/_/  /_/\____/_/  /_/\____/_/  /_/\__,_/   )"<< endl;
  cout <<  R"(                                           )"<< endl;
  cout << "Start time:            " << dateString() << "\n"
       << "Number of ranks:       " << MPI::globalNoDomains() << "\n"
       #ifdef _OPENMP
       << "Number of OMP threads: " << omp_get_max_threads() << "\n"
       #endif
      // << "Host (of rank 0):      " << host << "\n"
     //  << "Working directory:     " << dir << "\n"
      // << "User:                  " << user << "\n"
      // << "Executable:            " << executable << "\n"
       << endl;
}

template <GInt DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::loadConfiguration() {
  gridgen_log << "Loading configuration file..." << endl;

}

template <GInt DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::generateGrid() {
  gridgen_log << "Generating a grid..." << endl;

}



template class GRIDGEN::GridGenerator<0>;
template class GRIDGEN::GridGenerator<1>;
template class GRIDGEN::GridGenerator<2>;
template class GRIDGEN::GridGenerator<3>;
template class GRIDGEN::GridGenerator<4>;