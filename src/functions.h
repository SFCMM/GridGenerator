#ifndef GRIDGENERATOR_FUNCTIONS_H
#define GRIDGENERATOR_FUNCTIONS_H
#include <sstream>
#include "constants.h"

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

/// Return maximum number of Neighbors per cell
template <GInt NDIM>
static constexpr inline auto maxNoNghbrs() -> GInt {
  return 2 * NDIM;
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
