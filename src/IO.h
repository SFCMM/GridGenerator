#ifndef GRIDGENERATOR_IO_H
#define GRIDGENERATOR_IO_H
#include <fstream>
#include <iostream>
#include "common/types.h"
#include "constants.h"
#include "macros.h"
//#include <csv/csv.hpp>

namespace hidden::_detail {
const std::function<GBool(GInt)> defaultTrue  = [](GInt /*ignored*/) { return true; };
const std::function<GBool(GInt)> defaultFalse = [](GInt /*ignored*/) { return false; };
} // namespace hidden::_detail

namespace ASCII {
// using namespace csv;
using namespace std;

template <GInt DIM>
inline void writePointsCSV(const GString& fileName, const GInt noValues, const std::vector<VectorD<DIM>>& coordinates,
                           const std::vector<GString>& index = {}, const std::vector<std::vector<GString>>& values = {},
                           const std::function<GBool(GInt)>& filter = hidden::_detail::defaultTrue) {
  ASSERT(index.size() == values.size(), "Invalid values/index size!");

  ofstream                      pointFile;
  static constexpr unsigned int N           = 64;
  static constexpr unsigned int buffer_size = 1024 * N;
  std::array<char, buffer_size> buffer{};
  pointFile.rdbuf()->pubsetbuf(&buffer[0], buffer_size);
  pointFile.open(fileName + ".csv");

  for(GInt id = 0; id < DIM; ++id) {
    pointFile << coordinate::name.at(id);
    if(id + 1 < DIM) {
      pointFile << ",";
    }
  }
  for(const auto& columnHeader : index) {
    pointFile << "," << columnHeader;
  }
  pointFile << "\n";

  for(GInt id = 0; id < noValues; ++id) {
    if(!filter(id)) {
      continue;
    }
    const auto& coord = coordinates[id];
    for(GInt i = 0; i < DIM; ++i) {
      pointFile << coord[i];
      if(i + 1 < DIM) {
        pointFile << ",";
      }
    }
    for(const auto& column : values) {
      pointFile << "," << column[id];
    }
    pointFile << "\n";
  }
  pointFile.close();
}

} // namespace ASCII

namespace VTK {}

namespace HDF5 {}

#endif // GRIDGENERATOR_IO_H
