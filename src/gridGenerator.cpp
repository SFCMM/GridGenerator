#include <gcem.hpp>
#include <iostream>
#include <mpi.h>
#include <utility>

#include "config.h"
#include "constants.h"
#include "functions.h"
#include "geometry.h"
#include "globalmpi.h"
#include "gridGenerator.h"
#include "sys.h"
#include "timer.h"

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace gridgen;
using namespace std;

template <Debug_Level DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::init(int argc, GChar** argv, GString config_file) {
  m_exe                   = argv[0]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  m_configurationFileName = std::move(config_file);

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
  if(MPI::isRoot()) {
    cerr0.rdbuf(std::cerr.rdbuf());
  } else {
    cerr0.rdbuf(&nullBuffer);
  }

#ifndef GRIDGEN_SINGLE_FILE_LOG
  gridgen_log.open("gridgen_log" + std::to_string(m_domainId), false, argc, argv, MPI_COMM_WORLD);
#else
  if(DEBUG_LEVEL < Debug_Level::more_debug) {
    gridgen_log.open("gridgen_log", true, argc, argv, MPI_COMM_WORLD);
  } else {
    gridgen_log.open("gridgen_log", false, argc, argv, MPI_COMM_WORLD);
  }
#endif
  gridgen_log.setMinFlushSize(LOG_MIN_FLUSH_SIZE);

  initTimers();
}

template <Debug_Level DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::initTimers() {
  RESET_TIMERS();

  NEW_TIMER_GROUP_NOCREATE(TimeKeeper[Timers::AppGroup], "Application");
  NEW_TIMER_NOCREATE(TimeKeeper[Timers::timertotal], "Total", TimeKeeper[Timers::AppGroup]);
  RECORD_TIMER_START(TimeKeeper[Timers::timertotal]);

  NEW_SUB_TIMER_NOCREATE(TimeKeeper[Timers::Init], "Init", TimeKeeper[Timers::timertotal]);
  NEW_SUB_TIMER_NOCREATE(TimeKeeper[Timers::GridGeneration], "Create the grid.", TimeKeeper[Timers::timertotal]);
  NEW_SUB_TIMER_NOCREATE(TimeKeeper[Timers::GridInit], "Init grid.", TimeKeeper[Timers::GridGeneration]);
  NEW_SUB_TIMER_NOCREATE(TimeKeeper[Timers::GridPart], "Partitioning grid generation.",
                         TimeKeeper[Timers::GridGeneration]);
  NEW_SUB_TIMER_NOCREATE(TimeKeeper[Timers::GridUniform], "Uniform grid generation.",
                         TimeKeeper[Timers::GridGeneration]);
  NEW_SUB_TIMER_NOCREATE(TimeKeeper[Timers::GridRefinement], "Grid refinement.", TimeKeeper[Timers::GridGeneration]);
  NEW_TIMER_NOCREATE(TimeKeeper[Timers::IO], "IO", TimeKeeper[Timers::timertotal]);
}

template <Debug_Level DEBUG_LEVEL>
auto GridGenerator<DEBUG_LEVEL>::run() -> int {
  RECORD_TIMER_START(TimeKeeper[Timers::Init]);
  startupInfo();
  gridgen_log << "Grid generator started ||>" << endl;
  cout << "Grid generator started ||>" << endl;
  loadConfiguration();
  RECORD_TIMER_STOP(TimeKeeper[Timers::Init]);

  // todo: replace with switch
  if(m_dim == 2) {
    generateGrid<2>();
  } else if(m_dim == 3) {
    generateGrid<3>();
  } else if(m_dim == 4) {
    generateGrid<4>();
  } else if(m_dim == 1) {
    generateGrid<1>();
  }
  gridgen_log << "Grid generator finished <||" << endl;
  cout << "Grid generator finished <||" << endl;

  unusedConfigValues();

  RECORD_TIMER_STOP(TimeKeeper[Timers::timertotal]);
  STOP_ALL_RECORD_TIMERS();
  DISPLAY_ALL_TIMERS();

  gridgen_log.close();
  MPI_Finalize();

  return 0;
}
template <Debug_Level DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::startupInfo() {
  if(MPI::isRoot()) {
    cout << R"(    __  _______  __  _________     _     __)" << endl;
    cout << R"(   /  |/  / __ \/  |/  / ____/____(_)___/ /)" << endl;
    cout << R"(  / /|_/ / / / / /|_/ / / __/ ___/ / __  / )" << endl;
    cout << R"( / /  / / /_/ / /  / / /_/ / /  / / /_/ /  )" << endl;
    cout << R"(/_/  /_/\____/_/  /_/\____/_/  /_/\__,_/   )" << endl;
    cout << R"(                                           )" << endl;
    cout << "Start time:            " << dateString() << "\n"
         << "Number of ranks:       " << MPI::globalNoDomains() << "\n"
#ifdef _OPENMP
         << "Number of OMP threads: " << omp_get_max_threads() << "\n"
#endif
         << "Host (of rank 0):      " << hostString() << "\n"
         << "Working directory:     " << getCWD() << "\n"
         << "Executable:            " << m_exe << "\n"
         << endl;
  }
}

template <Debug_Level DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::loadConfiguration() {
  RECORD_TIMER_START(TimeKeeper[Timers::IO]);

  gridgen_log << "Loading configuration file [" << m_configurationFileName << "]" << endl;

  // 1. open configuration file on root process
  if(MPI::isRoot()) {
    std::ifstream configFileStream(m_configurationFileName);
    configFileStream >> m_config;
    // range-based for
    for(auto& element : m_config.items()) {
      m_configKeys.emplace(element.key(), false);
    }
    configFileStream.close();
  }

  // 2. communicate configuration file to all other processes
  if(!MPI::isSerial()) {
    TERMM(-1, "Not implemented!");
  }

  // 3. load&check configuration values
  m_dim        = required_config_value<GInt>("dim");
  m_maxNoCells = required_config_value<GInt>("maxNoCells");

  m_dryRun          = opt_config_value<GBool>("dry-run", m_dryRun);
  m_outputDir       = opt_config_value<GString>("outputDir", m_outputDir);
  m_maxNoOffsprings = opt_config_value<GInt>("maxNoOffsprings", m_maxNoOffsprings);
  RECORD_TIMER_STOP(TimeKeeper[Timers::IO]);
}

template <Debug_Level DEBUG_LEVEL>
template <GInt NDIM>
void GridGenerator<DEBUG_LEVEL>::generateGrid() {
  RECORD_TIMER_START(TimeKeeper[Timers::GridGeneration]);
  RECORD_TIMER_START(TimeKeeper[Timers::GridInit]);

  gridgen_log << "Generating a grid[" << NDIM << "D]" << endl;
  m_grid = std::make_unique<CartesianGridGen<DEBUG_LEVEL, NDIM>>();

  cout << SP1 << "(1) Reading Grid definition" << endl;
  loadGridDefinition<NDIM>();
  m_grid->setCapacity(m_maxNoCells);
  gridgen_log << SP2 << "+ maximum number of cells: " << m_maxNoCells << endl;
  cout << SP2 << "+ maximum number of cells: " << m_maxNoCells << endl;

  // todo: add function to define memory to be allocated
  // todo: add function to convert to appropriate memory size
  const GDouble memoryConsumptionKB = CartesianGridGen<DEBUG_LEVEL, NDIM>::memorySizePerCell() * m_maxNoCells / DKBIT;
  if(MPI::isSerial()) {
    gridgen_log << SP2 << "+ memory allocated: " << memoryConsumptionKB << "KB" << std::endl;
    cout << SP2 << "+ memory allocated: " << memoryConsumptionKB << "KB" << std::endl;
  } else {
    gridgen_log << SP2 << "+ local memory allocated: " << memoryConsumptionKB << "KB" << std::endl;
    cout << SP2 << "+ local memory allocated: " << memoryConsumptionKB << "KB" << std::endl;
    const GDouble globalMemory = memoryConsumptionKB * static_cast<GDouble>(MPI::globalNoDomains());
    gridgen_log << SP2 << "+ global memory allocated: " << globalMemory << "KB" << std::endl;
    cout << SP2 << "+ global memory allocated: " << globalMemory << "KB" << std::endl;
  }

  m_grid->setMinLvl(m_partionLvl);
  m_grid->setMaxLvl(m_maxRefinementLvl);
  // todo: allow setting the weighting method
  m_weightMethod = std::make_unique<WeightUniform>();

  cout << SP1 << "(2) Reading Geometry" << endl;
  m_geometry = std::make_unique<Geometry<NDIM>>(MPI_COMM_WORLD);
  // todo: implement
  //  m_noBndIdsPerSolver.reserve(m_geometry->noNodes());
  //  for(MInt solver = 0; solver < m_geometry->noNodes(); solver++) {
  //    m_noBndIdsPerSolver[solver] = m_geometry->noSegmentsOfNode(solver);
  //  }
  //  m_bndCutInfo.reserve(m_geometry->noNodes());
  //  if(!has_config_value("has_config_value")){
  //    m_grid->setBoundingBox(m_geometry->boundingBox());
  //  }


  gridgen_log << "\n";
  gridgen_log << SP2 << "+ m_center of gravity: " << strStreamify<NDIM>(m_grid->cog()).str() << "\n";
  gridgen_log << SP2 << "+ decisive direction: " << m_grid->decisiveDirection() << "\n";
  gridgen_log << SP2 << "+ geometry extents: " << strStreamify<NDIM>(m_grid->geomExtent()).str() << "\n";
  gridgen_log << SP2 << "+ bounding box: " << strStreamify<2 * NDIM>(m_grid->boundingBox()).str() << endl;
  RECORD_TIMER_STOP(TimeKeeper[Timers::GridInit]);

  m_grid->createPartitioningGrid();

  RECORD_TIMER_START(TimeKeeper[Timers::GridUniform]);
  GInt                     x  = 1;
  static constexpr GDouble pi = 3.1415;
  for(int i = 0; i < 10000; ++i) {
    x += gcem::lgamma(2 * pi * NDIM);
  }
  RECORD_TIMER_STOP(TimeKeeper[Timers::GridUniform]);


  RECORD_TIMER_START(TimeKeeper[Timers::GridRefinement]);
  GInt y = 1;
  for(int i = 0; i < 10000; ++i) {
    y += gamma(2 * pi * NDIM);
  }
  RECORD_TIMER_STOP(TimeKeeper[Timers::GridRefinement]);

  RECORD_TIMER_STOP(TimeKeeper[Timers::GridGeneration]);
}

template <Debug_Level DEBUG_LEVEL>
template <GInt NDIM>
void GridGenerator<DEBUG_LEVEL>::loadGridDefinition() {
  RECORD_TIMER_START(TimeKeeper[Timers::IO]);

  m_partionLvl = required_config_value<GInt>("partionLevel");
  m_uniformLvl = required_config_value<GInt>("uniformLevel");

  m_grid->setBoundingBox(opt_config_value<std::vector<GDouble>>("boundingBox", DEFAULT_BOUNDINGBOX.at(m_dim - 1)));

  m_maxRefinementLvl = m_uniformLvl;
  m_maxRefinementLvl = opt_config_value<GInt>("maxRfnmtLvl", m_maxRefinementLvl);

  m_outGridFilename = opt_config_value<GString>("gridFileName", m_outGridFilename);
  RECORD_TIMER_STOP(TimeKeeper[Timers::IO]);
}

template <Debug_Level DEBUG_LEVEL>
template <typename T>
auto GridGenerator<DEBUG_LEVEL>::required_config_value(const GString& key) -> T {
  if(m_config.template contains(key)) {
    m_configKeys[key] = true;
    return static_cast<T>(m_config[key]);
  }
  TERMM(-1, "The required configuration value is missing: " + key);
}

template <Debug_Level DEBUG_LEVEL>
template <typename T>
auto GridGenerator<DEBUG_LEVEL>::opt_config_value(const GString& key, const T& defaultValue) -> T {
  if(m_config.template contains(key)) {
    m_configKeys[key] = true;
    return static_cast<T>(m_config[key]);
  }
  return defaultValue;
}

template <Debug_Level DEBUG_LEVEL>
auto GridGenerator<DEBUG_LEVEL>::has_config_value(const GString& key) -> GBool {
  if(m_config.template contains(key)) {
    return true;
  }
  TERMM(-1, "The required configuration value is missing: " + key);
}


template <Debug_Level DEBUG_LEVEL>
void GridGenerator<DEBUG_LEVEL>::unusedConfigValues() {
  GInt i = 0;
  gridgen_log << "The following values in the configuration file are unused:" << endl;
  for(const auto& configKey : m_configKeys) {
    if(!configKey.second) {
      gridgen_log << "[" << ++i << "] " << configKey.first << "\n";
    }
  }
  gridgen_log << endl;
}


template class gridgen::GridGenerator<Debug_Level::no_debug>;
template class gridgen::GridGenerator<Debug_Level::min_debug>;
template class gridgen::GridGenerator<Debug_Level::debug>;
template class gridgen::GridGenerator<Debug_Level::more_debug>;
template class gridgen::GridGenerator<Debug_Level::max_debug>;