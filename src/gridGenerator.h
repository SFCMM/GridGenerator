#ifndef GRIDGENERATOR_GRIDGENERATOR_H
#define GRIDGENERATOR_GRIDGENERATOR_H

#include <ostream>
#include <json.h>

#include "global.h"
#include "types.h"

using json = nlohmann::json;

extern std::ostream cerr0; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

namespace GRIDGEN {
class NullBuffer : public std::streambuf {
 public:
  auto overflow(int c) -> int override { return c; }
};

template <GInt DEBUG_LEVEL>
class GridGenerator {
 public:
  GridGenerator()                     = default;
  ~GridGenerator()                    = default;
  GridGenerator(const GridGenerator&) = delete;
  GridGenerator(GridGenerator&&)      = delete;
  auto operator=(const GridGenerator&) -> GridGenerator& = delete;
  auto operator=(GridGenerator&&) -> GridGenerator& = delete;


  void init(int argc, GChar** argv);
  auto run() -> int;

  void showLogo();
  void loadConfiguration();
  void generateGrid();

 private:
  NullBuffer nullBuffer;

  int m_domainId  = -1;
  int m_noDomains = -1;

  GString configurationFileName="grid.json";

  json config;
};

} // namespace GRIDGEN

#endif // GRIDGENERATOR_GRIDGENERATOR_H
