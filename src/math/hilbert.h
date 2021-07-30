#ifndef GRIDGENERATOR_HILBERT_H
#define GRIDGENERATOR_HILBERT_H
#include "types.h"
#include <bitset>
#include <gcem.hpp>

namespace hilbert {
/// Calculate the Hilbert index of a given coordinate up to the HilberLevel iteration.
/// \tparam NDIM Dimension of the coordinates. (Currently limited to 4)
/// \param x Coordinates in unit square coordinates in (0,1)
/// \param hilbertLevel The number of iterations of the Hilbert curve.
/// \return The Hilbert index. Which should be unique for cells in a valid Cartesian grid.
template <GInt NDIM>
inline auto index(const VectorD<NDIM>& x, const GInt hilbertLevel) -> GInt {
  // todo: make this assert work
  //    ASSERT(static_cast<GBool>(x.array() >=0) && static_cast<GBool>(x.array() <=1), "Invalid Coordinates");
  VectorD<NDIM> position = x;
  GInt          index    = 0;

  for(GInt level = 0; level < hilbertLevel; ++level) {
    std::bitset<NDIM> quadrant;
    for(GInt dir = 0; dir < NDIM; ++dir) {
      quadrant[dir] = position[dir] >= 0.5;
    }
    const GInt hilbertLUTId = quadrant.to_ulong();
    static_assert(NDIM <= 4, "Not implemented!");
    // todo: find a more general way (maybe) this is faster than anything else...
    static constexpr std::array<GInt, 16> hilbertLUT{0, 3, 1, 2, 5, 4, 6, 7, 10, 9, 11, 8, 15, 14, 12, 13};


    // Skip the maximum number of cells in the subtrees given by the hilbertLevel
    // 2D: hilbertLevel =3
    // l=0: 2^(2*2)=16
    // l=1: 2^(2)=4
    // l=2: 2^0=1
    const GInt multiplier = gcem::pow(2, NDIM * (hilbertLevel - 1 - level));
    GInt       quad       = hilbertLUT[hilbertLUTId];
    index += multiplier * quad;

    // rescale to new unit cube of half the size!
    VectorD<NDIM> transformed = 2 * position;
    if(quad >= 8) {
      if(transformed[2] > 1) {
        transformed[2] -= 1;
      }
      transformed[3] -= 1;
      // rotation of the curve
      if(quad >= 12) {
        --quad;
      }
      quad = (quad - 6) % 4;
    }
    if(quad >= 4) {
      transformed[2] -= 1;
      // rotation of the curve
      quad = quad % 4 - 1;
      if(quad == -1) {
        transformed[0] -= 1;
      }
    }
    switch (quad){
      case 1:
        transformed[1] -= 1;
        break;
      case 2:
        transformed[0] -= 1;
        transformed[1] -= 1;
        break;
      case 3:
        transformed[0] -= 1;
        break;
      default:
        break;
    }
    position = transformed;
  }
  return index;
}
} // namespace hilbert
#endif // GRIDGENERATOR_HILBERT_H