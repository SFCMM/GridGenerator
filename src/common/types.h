#ifndef GRIDGENERATOR_TYPES_H
#define GRIDGENERATOR_TYPES_H
#include <cstdint>
#include <string>
#include "compiler_config.h"
#ifdef GCC_COMPILER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wduplicated-branches"
#endif
#ifdef CLANG_COMPILER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wc99-extensions"
#endif
#include <Eigen/Core>
#ifdef GCC_COMPILER
#pragma GCC diagnostic pop
#endif
#ifdef CLANG_COMPILER
#pragma clang diagnostic pop
#endif


using GFloat      = float;
using GDouble     = double;
using GLongDouble = long double;

using GString = std::basic_string<char>;
using GChar   = char;
using GUchar  = unsigned char;
using GBool   = bool;

using GInt  = int64_t;
using GUint = uint64_t;

template <GInt NDIM>
using VectorD = Eigen::Matrix<GDouble, NDIM, 1>;
template <GInt NDIM>
using VectorI = Eigen::Matrix<GInt, NDIM, 1>;

class NullBuffer : public std::streambuf {
 public:
  auto overflow(int c) -> int override { return c; }
};

inline NullBuffer nullBuffer; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

#endif // GRIDGENERATOR_TYPES_H
