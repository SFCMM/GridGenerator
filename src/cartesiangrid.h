#ifndef GRIDGENERATOR_CARTESIANGRID_H
#define GRIDGENERATOR_CARTESIANGRID_H

#include "macros.h"

class GridInterface{
 public:
  virtual void setBoundingBox(std::vector<GDouble> bbox) = 0;
};

template<Debug_Level DebugLevel, GInt NDIM>
class CartesianGrid: public GridInterface {
 public:
  void setBoundingBox(std::vector<GDouble> bbox) override{
    if(bbox.size() != 2*NDIM){
      TERMM(-1, "Invalid boundary box definition.");
    }
    std::copy_n(bbox.begin(), 2*NDIM, m_boundingBox.begin());
  }
 private:
  std::array<GDouble, 2*NDIM>  m_boundingBox{NAN};
};

#endif // GRIDGENERATOR_CARTESIANGRID_H
