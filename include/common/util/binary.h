// SPDX-License-Identifier: BSD-3-Clause

#ifndef COMMON_BINARY_H
#define COMMON_BINARY_H

#include <bitset>

namespace binary {
template <typename T>
static constexpr inline std::bitset<sizeof(T) * 8> convert(const T num) {
  T tmp = num;
  return std::bitset<sizeof(T) * 8>(
      *(static_cast<uint64_t *>(static_cast<void *>(&tmp))));
}

template <typename T> static constexpr inline void swapEndian(T &val) {
  union U {
    T val;
    std::array<std::uint8_t, sizeof(T)> raw;
  } src, dst;

  src.val = val;
  std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
  val = dst.val;
}

template <typename T>
static constexpr inline auto getSwappedEndian(const T val) {
  T tmp = val;
  swapEndian(tmp);
  return tmp;
}

template <>
constexpr inline void swapEndian<std::uint16_t>(std::uint16_t &value) {
  value = (value >> 8) | (value << 8);
}

template <>
constexpr inline void swapEndian<std::uint32_t>(std::uint32_t &value) {
  std::uint32_t tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
  value = (tmp << 16) | (tmp >> 16);
}

template <>
constexpr inline void swapEndian<std::uint64_t>(std::uint64_t &value) {
  value = ((value & 0x00000000FFFFFFFFull) << 32) |
          ((value & 0xFFFFFFFF00000000ull) >> 32);
  value = ((value & 0x0000FFFF0000FFFFull) << 16) |
          ((value & 0xFFFF0000FFFF0000ull) >> 16);
  value = ((value & 0x00FF00FF00FF00FFull) << 8) |
          ((value & 0xFF00FF00FF00FF00ull) >> 8);
}

template <typename T>
static constexpr inline std::bitset<sizeof(T) * 8> convertSwap(const T num) {
  T tmp = num;
  swapEndian(tmp);
  return convert(tmp);
}

} // namespace binary

#endif // COMMON_BINARY_H
