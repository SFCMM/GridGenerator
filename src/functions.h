#ifndef GRIDGENERATOR_FUNCTIONS_H
#define GRIDGENERATOR_FUNCTIONS_H
#include <bitset>
#include <gcem.hpp>
#include <iostream>
#include <json.h>
#include <map>
#include <sstream>
#include <sfcmm_common.h>
#include "term.h"

namespace config {
using json = nlohmann::json;
template <typename T>
static constexpr inline auto required_config_value(const json& config, const GString& key) -> T {
  // todo: check for types
  if(config.template contains(key)) {
    return static_cast<T>(config[key]);
  }
  TERMM(-1, "The required configuration value is missing: " + key);
}

template <typename T>
static constexpr inline auto opt_config_value(const json& config, const GString& key, const T& defaultValue) -> T {
  // todo: check for types
  if(config.template contains(key)) {
    return static_cast<T>(config[key]);
  }
  return defaultValue;
}
} // namespace config

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
