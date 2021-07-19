#include <iostream>
#include <mpi.h>
#include <gcem.hpp>

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

  initTimers();
}

template <GInt DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::initTimers() {
  RESET_TIMERS();

  NEW_TIMER_GROUP_NOCREATE(m_timers[Timers::AppGroup], "Application");
  NEW_TIMER_NOCREATE(m_timers[Timers::timertotal], "Total", m_timers[Timers::AppGroup]);
  RECORD_TIMER_START(m_timers[Timers::timertotal]);

  NEW_SUB_TIMER_NOCREATE(m_timers[Timers::Init], "Init", m_timers[Timers::timertotal]);
  NEW_SUB_TIMER_NOCREATE(m_timers[Timers::GridGeneration], "Create the grid.", m_timers[Timers::timertotal]);
  NEW_SUB_TIMER_NOCREATE(m_timers[Timers::GridUniform], "Uniform grid generation.", m_timers[Timers::GridGeneration]);
  NEW_SUB_TIMER_NOCREATE(m_timers[Timers::GridRefinement], "Grid refinement.", m_timers[Timers::GridGeneration]);
  NEW_TIMER_NOCREATE(m_timers[Timers::IO], "IO", m_timers[Timers::timertotal]);
}

template <GInt DEBUG_LEVEL>
auto GridGenerator<DEBUG_LEVEL>::run() -> int {
  RECORD_TIMER_START(m_timers[Timers::Init]);

  gridgen_log << "Grid generator started ||>"<<endl;
  startupInfo();

  RECORD_TIMER_START(m_timers[Timers::IO]);

  loadConfiguration();

  RECORD_TIMER_STOP(m_timers[Timers::IO]);
  RECORD_TIMER_STOP(m_timers[Timers::Init]);
  RECORD_TIMER_START(m_timers[Timers::GridGeneration]);

  generateGrid();
  gridgen_log << "Grid generator finished <||"<<endl;

  RECORD_TIMER_STOP(m_timers[Timers::GridGeneration]);
  RECORD_TIMER_STOP(m_timers[Timers::timertotal]);
  STOP_ALL_RECORD_TIMERS();
  DISPLAY_ALL_TIMERS();

  gridgen_log.close();
  MPI_Finalize();

  return 0;
}
template <GInt DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::startupInfo() {
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

  RECORD_TIMER_START(m_timers[Timers::GridUniform]);
  GInt x = 1;
  static constexpr GDouble pi = 3.1415;
  for(int i = 0; i < 10000; i++){
    x+=gcem::lgamma(2*pi);
  }
  RECORD_TIMER_STOP(m_timers[Timers::GridUniform]);


  RECORD_TIMER_START(m_timers[Timers::GridRefinement]);
  GInt y = 1;
  for(int i = 0; i < 10000; i++){
    y+=gamma(2*pi);
  }
  RECORD_TIMER_STOP(m_timers[Timers::GridRefinement]);

}



template class GRIDGEN::GridGenerator<0>;
template class GRIDGEN::GridGenerator<1>;
template class GRIDGEN::GridGenerator<2>;
template class GRIDGEN::GridGenerator<3>;
template class GRIDGEN::GridGenerator<4>;