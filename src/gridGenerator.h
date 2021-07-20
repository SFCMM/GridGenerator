#ifndef GRIDGENERATOR_GRIDGENERATOR_H
#define GRIDGENERATOR_GRIDGENERATOR_H

#include <json.h>
#include <ostream>

#include "cartesiangrid.h"
#include "globaltimers.h"
#include "types.h"

using json = nlohmann::json;

extern std::ostream cerr0; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

namespace GRIDGEN {
class NullBuffer : public std::streambuf {
 public:
  auto overflow(int c) -> int override { return c; }
};

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
  using Timers = gridgenerator::Timers_;
  std::array<GInt, Timers::_count> m_timers{};

  NullBuffer nullBuffer;

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
  void unusedConfigValues();


  template <typename T>
  auto required_config_value(const GString& key) -> T;
  template <typename T>
  auto opt_config_value(const GString& key, const T& defaultValue) -> T;


  GInt    m_dim              = -1;
  GInt    m_uniformLvl       = -1;
  GInt    m_maxNoCells       = -1;
  GInt    m_maxRefinementLvl = -1;
  GBool   m_dryRun           = false;
  GString m_outputDir        = "out";

  std::unique_ptr<GridInterface> m_grid;
};

} // namespace GRIDGEN

#endif // GRIDGENERATOR_GRIDGENERATOR_H
