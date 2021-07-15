#ifndef GRIDGENERATOR_GRIDGENERATOR_H
#define GRIDGENERATOR_GRIDGENERATOR_H

#include <ostream>
#include "global.h"
#include "types.h"


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


  auto run(int argc, GChar** argv) -> GInt;

 private:
  NullBuffer nullBuffer;

  int m_domainId  = -1;
  int m_noDomains = -1;
};

} // namespace GRIDGEN

#endif // GRIDGENERATOR_GRIDGENERATOR_H
