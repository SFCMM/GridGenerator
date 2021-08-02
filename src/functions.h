#ifndef GRIDGENERATOR_FUNCTIONS_H
#define GRIDGENERATOR_FUNCTIONS_H
#include <bitset>
#include <gcem.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include "common/types.h"
#include "constants.h"

/// Helpers
template <GInt LENGTH, class T>
static inline auto strStreamify(const std::vector<T>& in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

template <GInt LENGTH, class T>
static inline auto strStreamify(const std::array<T, LENGTH>& in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

template <typename T>
static inline auto toStringVector(const std::vector<T>& in, GInt size = -1) -> std::vector<GString> {
  std::vector<GString> string_vector;

  if(size == -1) {
    size = in.size();
  }

  std::transform(in.begin(), in.begin() + size, std::back_inserter(string_vector),
                 [](T b) -> GString { return std::to_string(b); });
  return string_vector;
}

template <>
inline auto toStringVector<std::byte>(const std::vector<std::byte>& in, GInt size) -> std::vector<GString> {
  std::vector<GString> string_vector;

  if(size == -1) {
    size = in.size();
  }

  std::transform(in.begin(), in.begin() + size, std::back_inserter(string_vector),
                 [](std::byte b) -> GString { return std::to_string(std::to_integer<GInt>(b)); });
  return string_vector;
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






inline auto checkDuplicateIds(const std::vector<GInt>& ids) -> std::vector<GInt> {
  std::unordered_map<GInt, GInt> countMap;

  // Iterate over the vector and store the frequency of each element in map
  for(const auto& elem : ids) {
    const auto [it, success] = countMap.insert(std::pair<GInt, GInt>(elem, 1));
    if(!success) {
      // todo: using *it* produces a warning... (7/2021)
      countMap[elem]++;
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
