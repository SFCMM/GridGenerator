#ifndef GRIDGENERATOR_GMATH_H
#define GRIDGENERATOR_GMATH_H

#include <cmath>
#include "types.h"

namespace detail_ {
template <class T, class U>
struct APPROX_ERROR {};
} // namespace detail_

template <class T, class U>
auto approx(const T& /*unused*/, const U& /*unused*/, const T /*unused*/) -> GBool {
  using error = typename detail_::APPROX_ERROR<T, U>::ERROR_BOTH_TYPES_MUST_BE_MFloats;
  error();
  return true;
}

template <>
inline auto approx<GDouble, GDouble>(const GDouble& a, const GDouble& b, const GDouble eps) -> GBool {
  return std::fabs(a - b) < eps;
}

#endif // GRIDGENERATOR_GMATH_H
