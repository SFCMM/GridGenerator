#ifndef GRIDGENERATOR_GRIDGENERATOR_H
#define GRIDGENERATOR_GRIDGENERATOR_H

#include <json.h>
#include <ostream>

#include "cartesiangrid.h"
#include "common/types.h"
#include "geometry.h"
#include "globaltimers.h"
#include "gridcell_properties.h"
#include "WeightMethod.h"

using json = nlohmann::json;

template <Debug_Level DEBUG_LEVEL>
class GridGenerator {
 public:
  GridGenerator()                     = default;
  ~GridGenerator()                    = default;
  GridGenerator(const GridGenerator&) = delete;
  GridGenerator(GridGenerator&&)      = delete;
  auto operator=(const GridGenerator&) -> GridGenerator& = delete;
  auto operator=(GridGenerator&&) -> GridGenerator& = delete;


  void init(int argc, GChar** argv, GString config_file);
  auto run() -> int;

 private:
  int m_domainId  = -1;
  int m_noDomains = -1;

  GString m_configurationFileName = "grid.json";
  GString m_exe;

  json                               m_config;
  std::unordered_map<GString, GBool> m_configKeys{};

  void initTimers();
  void startupInfo();
  void loadConfiguration();
  template <GInt nDim>
  void loadGridDefinition();
  template <GInt nDim>
  void generateGrid();


  template <typename T>
  auto required_config_value(const GString& key) -> T;
  template <typename T>
  auto opt_config_value(const GString& key, const T& defaultValue) -> T;
  auto has_config_value(const GString& key) -> GBool;
  void unusedConfigValues();


  GInt m_dim        = -1;
  GInt m_maxNoCells = -1;
  // maximum size of subtree of a partitioning cell
  GInt m_maxNoOffsprings = DEFAULT_MAXNOOFFSPRINGS;
  // maximum workload of subtree of a partitioning cell
  GInt                               m_maxOffspringWorkload = DEFAULT_MAXNOOFFSPRINGS;
  GInt                               m_partitionLvl         = -1;
  GInt                               m_uniformLvl           = -1;
  GInt                               m_maxRefinementLvl     = -1;
  GBool                              m_dryRun               = false;
  GString                            m_outputDir            = "out";
  GString                            m_outGridFilename      = "grid";
  std::unique_ptr<WeightMethod>      m_weightMethod;
  std::unique_ptr<GridInterface>     m_grid;
  std::shared_ptr<GeometryInterface> m_geometry;
  json                               m_geometryConfig;
};

#endif // GRIDGENERATOR_GRIDGENERATOR_H
