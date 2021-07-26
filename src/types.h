#ifndef GRIDGENERATOR_TYPES_H
#define GRIDGENERATOR_TYPES_H
#include <cstdint>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wduplicated-branches"
#include <Eigen/dense>
#pragma GCC diagnostic pop


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
using vectorD = Eigen::Matrix<GDouble, NDIM, 1>;

#endif // GRIDGENERATOR_TYPES_H
