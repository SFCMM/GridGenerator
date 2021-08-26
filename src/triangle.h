#ifndef GRIDGENERATOR_TRIANGLE_H
#define GRIDGENERATOR_TRIANGLE_H
namespace triangle_ {
template <GInt NDIM>
using Point = VectorD<NDIM>;
}

template <GInt NDIM>
struct triangle {
  std::array<triangle_::Point<NDIM>, 3> m_vertices;
  triangle_::Point<NDIM>                m_normal;
  // bounding box
  triangle_::Point<NDIM> m_max;
  triangle_::Point<NDIM> m_min;
};
namespace triangle_ {
template <GInt NDIM>
[[nodiscard]] inline auto min(const triangle<NDIM>& tri, const GInt dir) -> GDouble {
  return tri.m_min[dir];
}
template <GInt NDIM>
[[nodiscard]] inline auto max(const triangle<NDIM>& tri, const GInt dir) -> GDouble {
  return tri.m_max[dir];
}
template <GInt NDIM>
[[nodiscard]] inline auto countLineIntersections(const triangle<NDIM>& tri, const std::array<GDouble, 2 * NDIM>& line) -> GInt {
  return 1;
}

template <GInt NDIM>
[[nodiscard]] inline auto boundingBox(const triangle<NDIM>& tri, const GInt dir) -> GDouble {
  ASSERT(dir <= 2 * NDIM, "Invalid dir");

  if(isEven(dir)) {
    return tri.m_min(dir / 2);
  }
  return tri.m_max(dir / 2 + 1);
}

// template <GInt NDIM>
//[[nodiscard]] inline auto pointInBB(const triangle<NDIM>& tri, const Point<NDIM>& x) -> GBool {
//   for(GInt dir = 0; dir < NDIM; ++dir){
//     if(x[dir]>tri.m_max[dir] || x[dir]<tri.m_min[dir] ){
//       cerr0 << "dir" << dir << " x " << x[dir] << " max " << tri.m_max[dir] << " min " << tri.m_min[dir] <<std::endl;
//       return false;
//     }
//   }
//   return true;
// }
} // namespace triangle_
#endif // GRIDGENERATOR_TRIANGLE_H