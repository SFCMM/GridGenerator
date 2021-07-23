#ifndef GRIDGENERATOR_FUNCTIONS_H
#define GRIDGENERATOR_FUNCTIONS_H
#include <sstream>

template <GInt LENGTH, class T>
inline auto strStreamify(std::vector<T> in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

/// Return maximum number of children per cell
template <GInt NDIM>
inline auto maxNoChildren() -> GInt {
  return gcem::pow(2, NDIM);
}


#endif // GRIDGENERATOR_FUNCTIONS_H
