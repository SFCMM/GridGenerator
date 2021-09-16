#ifndef GRIDGENERATOR_IO_H
#define GRIDGENERATOR_IO_H
#include <fstream>
#include <iostream>
#include <sfcmm_common.h>
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

namespace ASCII {
// todo: combine to functions ASCII and binary!!
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
} // namespace ASCII

namespace BINARY {
// todo: combine to functions ASCII and binary!!
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
      "<Points>\n<DataArray type=\"Float32\" Name=\"Points\" NumberOfComponents=\"" + to_string(DIM) + "\" format=\"binary\"> \n";

  static constexpr string_view point_footer = "       </DataArray>\n"
                                              "      </Points>";

  static constexpr string_view vert_header = "      <Verts>\n"
                                             "        <DataArray type=\"Int64\" Name=\"connectivity\" format=\"binary\"> \n";

  static constexpr string_view offset_data_header = "        <DataArray type=\"Int64\" Name=\"offsets\" format=\"ASCII\"> \n";
  static constexpr string_view data_footer        = "        </DataArray>";
  static constexpr string_view vert_footer        = "        </Verts> \n";
  static constexpr string_view point_data_header  = "      <PointData> \n";
  static constexpr string_view point_data_footer  = "      </PointData> \n";
  auto                         point_data_int32   = [](const GString& name) {
    return GString("<DataArray type=\"Int32\" Name=\"" + name + "\" format=\"ASCII\">\n");
  };

  ofstream                                         pointFile;
  static constexpr unsigned int                    N           = 64;
  static constexpr unsigned int                    buffer_size = 1024 * N;
  static constexpr std::array<std::string_view, 4> padders     = {"", "=", "==", "==="};

  std::array<char, buffer_size> buffer{};
  pointFile.rdbuf()->pubsetbuf(&buffer[0], buffer_size);
  pointFile.open(fileName + ".vtp");

  pointFile << header;
  pointFile << piece_header;
  pointFile << point_header;

  {
    GInt                actualValues = 0;
    std::vector<GFloat> tmp_coords(noValues * DIM);

    for(GInt id = 0; id < noValues; ++id) {
      if(!filter(id)) {
        continue;
      }
      const auto& coord = coordinates[id];
      for(GInt i = 0; i < DIM; ++i) {
        tmp_coords[actualValues++] = coord[i];
      }
    }
    GInt       header_coord_size = 4 * actualValues;
    const GInt number_bytes      = 8 + header_coord_size;
    const GInt number_chars      = static_cast<GInt>(gcem::ceil(static_cast<GDouble>(number_bytes) * 8.0 / 6.0));
    const GInt padding           = 4 - (number_chars % 4);
    pointFile << base64::encodeLE<GInt, 1>(&header_coord_size);
    pointFile << base64::encodeLE<GFloat, 2>(&tmp_coords[0], actualValues) << padders[padding];
  }

  pointFile << "\n" << point_footer;
  pointFile << vert_header;
  {
    std::vector<GInt> tmp_id(noOutCells);
    for(GInt id = 0; id < noOutCells; ++id) {
      tmp_id[id] = id;
    }
    GInt       header_coord_size = binary::BYTE_SIZE * noOutCells;
    const GInt number_bytes      = binary::BYTE_SIZE + header_coord_size;
    const GInt number_chars      = static_cast<GInt>(gcem::ceil(number_bytes * 8.0 / 6.0));
    const GInt padding           = 4 - (number_chars % 4);
    pointFile << base64::encodeLE<GInt, 1>(&header_coord_size);
    pointFile << base64::encodeLE<GInt, 2>(&tmp_id[0], noOutCells) << padders[padding];
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
} // namespace BINARY
} // namespace VTK

namespace HDF5 {}

#endif // GRIDGENERATOR_IO_H
