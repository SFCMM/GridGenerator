#ifndef GRIDGENERATOR_FUNCTIONS_H
#define GRIDGENERATOR_FUNCTIONS_H
#include <bitset>
#include <gcem.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include "constants.h"
#include "types.h"


template <GInt LENGTH, class T>
inline auto strStreamify(const std::vector<T>& in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

template <GInt LENGTH, class T>
inline auto strStreamify(const std::array<T, LENGTH>& in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}


// todo: implement this...
// static constexpr inline auto nghbrInside(const GInt childId, const GInt dir) -> GInt{
//
//   switch (childId){
//     case 0:// -x,-y,-z,-zz
//       //2D
//       //dir = 1 neighbor right +x 1
//       //dir = 3 neighbor up +y 2
//       //3D
//       //dir = 5 neighbor up chilId + 2^(NDIM-1) = 4
//       //4D
//       //dir = 7 childId + 2^NDIM = 8
//     case 1: //+x -y -z -zz
//       //2D
//       //dir = 0 neighbor left +x 0
//       //dir = 3 neighbor up +y 3
//       //3D
//       //dir = 5 neighbor up chilId + 2^(NDIM-1) = 5
//       //4D
//       //dir = 8 childId + 2^NDIM = 9
//       default:
//       return -1;
//     }
// }

static constexpr inline auto oppositeDir(const GInt dir) -> GInt {
  return 2 * (dir / 2) + 1 - (dir % 2);
  // 0 = 1 ok
  // 1 = 0 ok
  // 2 = 3 ok
  //...
  // 5 = 4 ok
  // 6 = 7 ok
  // 7 = 6 ok
}

template <class T>
static constexpr inline auto isEven(T num) -> GBool {
  return num % 2 == 0;
}

template <GInt NDIM, class T, class U>
static constexpr inline void assign(T& lhs, U& rhs) {
  for(int i = 0; i < NDIM; i++) {
    lhs[i] = static_cast<T>(rhs[i]);
  }
}

template <GInt NDIM, class T, class U>
static constexpr inline void fill(T& lhs, U value) {
  for(int i = 0; i < NDIM; i++) {
    lhs[i] = static_cast<T>(value);
  }
}

namespace hilbert {
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
    if(quad == 1) {
      transformed[1] -= 1;
    }
    if(quad == 2) {
      transformed[0] -= 1;
      transformed[1] -= 1;
    }
    if(quad == 3) {
      transformed[0] -= 1;
    }
    position = transformed;
  }
  return index;
}
} // namespace hilbert

inline auto checkDuplicateIds(const std::vector<GInt>& ids) -> std::vector<GInt> {
  std::map<GInt, GInt> countMap;

  // Iterate over the vector and store the frequency of each element in map
  for(const auto& elem : ids) {
    const auto [it, success] = countMap.insert(std::pair<GInt, GInt>(elem, 1));
    if(!success) {
      it->second++;
    }
  }
  // Output for elements with more than count 1
  std::vector<GInt> duplicated;
  for(auto& it : countMap) {
    if(it.second > 1) {
      duplicated.emplace_back(it.first);
    }
  }
  return duplicated;
}
#endif // GRIDGENERATOR_FUNCTIONS_H
