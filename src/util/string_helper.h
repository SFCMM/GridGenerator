#ifndef GRIDGENERATOR_STRING_HELPER_H
#define GRIDGENERATOR_STRING_HELPER_H
#include <bitset>
#include <gcem.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include "common/types.h"

/// Get a stringstream from a given std::vector. Type needs to overload "<<".
/// \tparam LENGTH Length of the vector to be streamed.
/// \tparam T Type of the vector
/// \param in Vector to be stringstreamed
/// \return std::stringstream of the in vector.
template <GInt LENGTH, class T>
static inline auto strStreamify(const std::vector<T>& in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

/// Get a stringstream from a given std::array. Type needs to overload "<<".
/// \tparam LENGTH Length of the vector to be streamed.
/// \tparam T Type of the array
/// \param in array to be stringstreamed
/// \return std::stringstream of the in array.
template <GInt LENGTH, class T>
static inline auto strStreamify(const std::array<T, LENGTH>& in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

/// Get a stringstream from a given std::array. Type needs to overload "<<".
/// \tparam LENGTH Length of the vector to be streamed.
/// \tparam T Type of the array
/// \param in array to be stringstreamed
/// \return std::stringstream of the in array.
template <GInt LENGTH>
static inline auto strStreamify(const VectorD<LENGTH>& in) -> std::stringstream {
  std::stringstream str;
  str << in[0];
  for(GInt i = 1; i < LENGTH; i++) {
    str << " " << in[i];
  }
  return str;
}

/// Convert an input vector to a string vector of the same size or as per the given optional argument size.
/// \tparam T Type of the vector.
/// \param in Input vector to be stringified.
/// \param size (Default=same as input vector) Provide the size if you want partial stringification.
/// \return Vector of strings of the input vector.
template <typename T>
static inline auto toStringVector(const std::vector<T>& in, GInt size = -1) -> std::vector<GString> {
  std::vector<GString> string_vector;

  if(size == -1) {
    size = in.size();
  }

  std::transform(in.begin(), in.begin() + size, std::back_inserter(string_vector), [](T b) -> GString { return std::to_string(b); });
  return string_vector;
}

/// Convert an input byte vector to a string vector of the same size or as per the given optional argument size. <std::byte version>
/// \param in Input byte vector to be stringified.
/// \param size (Default=same as input vector) Provide the size if you want partial stringification.
/// \return Vector of strings of the input byte vector.
template <>
inline auto toStringVector<std::byte>(const std::vector<std::byte>& in, GInt size) -> std::vector<GString> {
  std::vector<GString> string_vector;

  if(size == -1) {
    size = static_cast<GInt>(in.size());
  }

  std::transform(in.begin(), in.begin() + size, std::back_inserter(string_vector),
                 [](std::byte b) -> GString { return std::to_string(std::to_integer<GInt>(b)); });
  return string_vector;
}

#endif // GRIDGENERATOR_STRING_HELPER_H
