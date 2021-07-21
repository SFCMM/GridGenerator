#ifndef GRIDGENERATOR_FUNCTIONS_H
#define GRIDGENERATOR_FUNCTIONS_H

template <GInt LENGTH, class T>
inline auto strStreamify(std::vector<T> in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}


#endif // GRIDGENERATOR_FUNCTIONS_H
