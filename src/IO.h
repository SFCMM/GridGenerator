#ifndef GRIDGENERATOR_IO_H
#define GRIDGENERATOR_IO_H
#include <fstream>
#include <iostream>
#include <sfcmm_common.h>
#include "constants.h"
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

  cerr0 << SP1 << "Writing " << fileName << ".csv" << std::endl;
  logger << SP1 << "Writing " << fileName << ".csv" << std::endl;

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

namespace VTK {
using namespace std;

template <GInt DIM>
inline void writePoints(const GString& fileName, const GInt noValues, const std::vector<VectorD<DIM>>& coordinates,
                        const std::vector<GString>& index = {}, const std::vector<std::vector<GString>>& values = {},
                        const std::function<GBool(GInt)>& filter = hidden::_detail::defaultTrue) {
  cerr0 << SP1 << "Writing " << fileName << ".vtp" << std::endl;
  logger << SP1 << "Writing " << fileName << ".vtp" << std::endl;

  GInt noOutCells = 0;
  for(GInt id = 0; id < noValues; ++id) {
    if(!filter(id)) {
      continue;
    }
    noOutCells++;
  }

  static constexpr string_view header = "<VTKFile type=\"PolyData\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt64\">\n"
                                        "  <PolyData>\n";

  static constexpr string_view footer = "    </Piece>\n"
                                        "  </PolyData>\n"
                                        "</VTKFile> \n";

  const GString piece_header = "<Piece NumberOfPoints=\"" + to_string(noOutCells)
                               + "\" NumberOfVerts=\"1\" NumberOfLines=\"0\" NumberOfStrips=\"0\" "
                                 "NumberOfPolys=\"0\" > \n";

  const GString point_header =
      "<Points>\n<DataArray type=\"Float64\" Name=\"Points\" NumberOfComponents=\"" + to_string(DIM) + "\" format=\"ascii\"> \n";

  static constexpr string_view point_footer = "        </DataArray>\n"
                                              "      </Points>";

  static constexpr string_view vert_header = "      <Verts>\n"
                                             "        <DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\"> \n";

  static constexpr string_view offset_data_header = "        <DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\"> \n";
  static constexpr string_view data_footer        = "        </DataArray>";
  static constexpr string_view vert_footer        = "        </Verts> \n";
  static constexpr string_view point_data_header  = "      <PointData> \n";
  static constexpr string_view point_data_footer  = "      </PointData> \n";
  auto                         point_data_int32   = [](const GString& name) {
    return GString("<DataArray type=\"Int32\" Name=\"" + name + "\" format=\"ascii\">\n");
  };

  ofstream                      pointFile;
  static constexpr unsigned int N           = 64;
  static constexpr unsigned int buffer_size = 1024 * N;
  std::array<char, buffer_size> buffer{};
  pointFile.rdbuf()->pubsetbuf(&buffer[0], buffer_size);
  pointFile.open(fileName + ".vtp");

  pointFile << header;
  pointFile << piece_header;
  pointFile << point_header;

  for(GInt id = 0; id < noValues; ++id) {
    if(!filter(id)) {
      continue;
    }
    const auto& coord = coordinates[id];
    for(GInt i = 0; i < DIM; ++i) {
      pointFile << coord[i];
      if(i + 1 < DIM) {
        pointFile << " ";
      }
    }
    pointFile << "\n";
  }

  pointFile << point_footer;
  pointFile << vert_header;
  for(GInt id = 0; id < noOutCells; ++id) {
    pointFile << id << "\n";
  }
  pointFile << data_footer;
  pointFile << offset_data_header;
  pointFile << noOutCells << "\n";
  pointFile << data_footer;
  pointFile << vert_footer;
  pointFile << point_data_header;
  GInt i = 0;
  for(const auto& column : values) {
    pointFile << point_data_int32(index[i++]);
    for(GInt id = 0; id < noValues; ++id) {
      if(!filter(id)) {
        continue;
      }
      pointFile << column[id] << "\n";
    }
    pointFile << data_footer;
  }
  pointFile << point_data_footer;
  pointFile << footer;
}
} // namespace VTK

namespace HDF5 {}

#endif // GRIDGENERATOR_IO_H
