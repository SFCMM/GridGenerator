#ifndef GRIDGENERATOR_IO_H
#define GRIDGENERATOR_IO_H
#include <fstream>
#include <iostream>
#include "constants.h"
#include "types.h"
//#include <csv/csv.hpp>

namespace ASCII {
//using namespace csv;
using namespace std;

template <GInt DIM>
inline void writePointsCSV(const GString& fileName, const GInt noValues, const std::vector<VectorD<DIM>>& coordinates) {
  ofstream pointFile;
  pointFile.open(fileName+".csv");

  for(GInt id = 0; id < DIM; ++id){
    pointFile<<coordinate::name.at(id);
    if(id+1<DIM){
      pointFile <<",";
    }
  }
  pointFile<<"\n";

  for(GInt id = 0; id< noValues; ++id){
    const auto& coord = coordinates[id];
    for(GInt i = 0; i< DIM; ++i){
      pointFile << coord[i];
      if(i+1<DIM){
        pointFile <<",";
      }
    }
    pointFile << "\n";
  }
  pointFile.close();
}

} // namespace ASCII

namespace VTK {}

namespace HDF5 {}

#endif // GRIDGENERATOR_IO_H
