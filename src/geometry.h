#ifndef GRIDGENERATOR_GEOMETRY_H
#define GRIDGENERATOR_GEOMETRY_H

#include <memory>
#include <mpi.h>
#include <vector>

template <GInt NDIM>
using Point = VectorD<NDIM>;

class GeometryInterface {
 public:
  GeometryInterface(const MPI_Comm comm) : m_comm(comm){};
  virtual ~GeometryInterface() = default;

  virtual void        setup()                                                                         = 0;
  virtual inline auto pointIsInside(const GDouble* x) const -> GBool                                  = 0;
  virtual inline auto cutWithCell(const GDouble* cellCenter, const GDouble cellLength) const -> GBool = 0;

 private:
  MPI_Comm m_comm;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometryRepresentation {
 public:
  GeometryRepresentation()          = default;
  virtual ~GeometryRepresentation() = default;

  [[nodiscard]] virtual inline auto pointIsInside(const Point<NDIM>& x) const -> GBool                        = 0;
  [[nodiscard]] virtual inline auto cutWithCell(const Point<NDIM>& center, GDouble cellLength) const -> GBool = 0;

 private:
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometrySTL : public GeometryRepresentation<DEBUG_LEVEL, NDIM> {
 public:
 private:
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometryAnalytical : public GeometryRepresentation<DEBUG_LEVEL, NDIM> {
 public:
 private:
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeomSphere : public GeometryAnalytical<DEBUG_LEVEL, NDIM> {
 public:
  GeomSphere(const Point<NDIM>& center, GDouble radius) : m_center(center), m_radius(radius){};

  [[nodiscard]] auto inline pointIsInside(const Point<NDIM>& x) const -> GBool {
    return (x - m_center).norm() < m_radius + GDoubleEps;
  }
  [[nodiscard]] inline auto cutWithCell(const Point<NDIM>& cellCenter, GDouble cellLength) const -> GBool {
    return (cellCenter - m_center).norm()
           < (m_radius + (gcem::sqrt(NDIM * gcem::pow(0.5 * cellLength, 2))) + GDoubleEps);
  }

 private:
  Point<NDIM> m_center{NAN};
  GDouble     m_radius = 0;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeomBox : public GeometryAnalytical<DEBUG_LEVEL, NDIM> {
 public:
 private:
  Point<NDIM> m_A{NAN};
  Point<NDIM> m_B{NAN};
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeomCube : public GeometryAnalytical<DEBUG_LEVEL, NDIM> {
 public:
 private:
  Point<NDIM> m_center{NAN};
  GDouble     length = 0;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometryManager : public GeometryInterface {
 public:
  GeometryManager(const MPI_Comm comm) : GeometryInterface(comm){};

  void setup() override {
    // for testing setup sphere
    Point<NDIM> center;
    center.fill(0);
    m_geomObj.emplace_back(std::make_unique<GeomSphere<DEBUG_LEVEL, NDIM>>(center, 0.5));
    Point<NDIM> center2;
    center2.fill(0);
    center2[0] = 0.5;
    m_geomObj.emplace_back(std::make_unique<GeomSphere<DEBUG_LEVEL, NDIM>>(center2, 0.5));
  }

  [[nodiscard]] auto inline pointIsInside(const GDouble* x) const -> GBool override {
    Point<NDIM> point(x);
    return pointIsInside(point);
  }

  [[nodiscard]] auto inline pointIsInside(const Point<NDIM>& point) const -> GBool {
    // \todo: check overall bounding box first
    for(const auto& obj : m_geomObj) {
      if(obj->pointIsInside(point)) {
        return true;
      }
    }
    return false;
  }

  [[nodiscard]] auto inline cutWithCell(const GDouble* cellCenter, const GDouble cellLength) const -> GBool override {
    Point<NDIM> center(cellCenter);
    return cutWithCell(center, cellLength);
  }

  [[nodiscard]] auto inline cutWithCell(const Point<NDIM>& cellCenter, const GDouble cellLength) const -> GBool {
    // \todo: check overall bounding box first
    for(const auto& obj : m_geomObj) {
      if(obj->cutWithCell(cellCenter, cellLength)) {
        return true;
      }
    }
    return false;
  }

 private:
  std::vector<std::unique_ptr<GeometryRepresentation<DEBUG_LEVEL, NDIM>>> m_geomObj;
};

#endif // GRIDGENERATOR_GEOMETRY_H
