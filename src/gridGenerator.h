#ifndef GRIDGENERATOR_GRIDGENERATOR_H
#define GRIDGENERATOR_GRIDGENERATOR_H

#include <ostream>

extern std::ostream cerr0;

namespace GRIDGEN {
class NullBuffer : public std::streambuf {
 public:
  int overflow(int c) override { return c; }
};

template <int DEBUG_LEVEL>
class GridGenerator {
 public:
  GridGenerator()                     = default;
  ~GridGenerator()                    = default;
  GridGenerator(const GridGenerator&) = delete;
  GridGenerator(GridGenerator&&)      = delete;
  auto operator=(const GridGenerator&) -> GridGenerator& = delete;
  auto operator=(GridGenerator&&) -> GridGenerator& = delete;


  auto run(int argc, char** argv) -> int;

 private:
  NullBuffer nullBuffer;

  int m_domainId;
  int m_noDomains;
};

} // namespace GRIDGEN

#endif // GRIDGENERATOR_GRIDGENERATOR_H
