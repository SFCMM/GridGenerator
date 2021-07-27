#ifndef GRIDGENERATOR_FUNCTIONS_H
#define GRIDGENERATOR_FUNCTIONS_H
#include <gcem.hpp>
#include <sstream>
#include "constants.h"
#include "types.h"


template <GInt LENGTH, class T>
inline auto strStreamify(std::vector<T> in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

template <GInt LENGTH, class T>
inline auto strStreamify(std::array<T, LENGTH> in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

/// Return maximum number of children per cell
template <GInt NDIM>
static constexpr inline auto maxNoChildren() -> GInt {
  return gcem::pow(2, NDIM);
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

/// Return maximum number of Neighbors per cell
template <GInt NDIM>
static constexpr inline auto maxNoNghbrs() -> GInt {
  return 2 * NDIM;
}

static constexpr inline auto oppositeDir(const GInt dir) -> GInt{
  return 2 * (dir / 2) + 1 - (dir % 2);
  //0 = 1 ok
  //1 = 0 ok
  //2 = 3 ok
  //...
  //5 = 4 ok
  //6 = 7 ok
  //7 = 6 ok
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

#endif // GRIDGENERATOR_FUNCTIONS_H
