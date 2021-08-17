#ifndef GRIDGENERATOR_GEOMETRY_H
#define GRIDGENERATOR_GEOMETRY_H

#include <json.h>
#include <memory>
#include <mpi.h>
#include <utility>
#include <vector>
#include <sfcmm_common.h>
#include "functions.h"

template <GInt NDIM>
using Point = VectorD<NDIM>;

using json = nlohmann::json;

class GeometryInterface {
 public:
  GeometryInterface(const MPI_Comm comm) : m_comm(comm){};
  virtual ~GeometryInterface()                = default;
  GeometryInterface(const GeometryInterface&) = delete;
  GeometryInterface(GeometryInterface&&)      = delete;
  auto operator=(const GeometryInterface&) -> GeometryInterface& = delete;
  auto operator=(GeometryInterface&&) -> GeometryInterface& = delete;

  virtual void                      setup(const json& geometry)                                                     = 0;
  virtual inline auto               pointIsInside(const GDouble* x) const -> GBool                                  = 0;
  virtual inline auto               cutWithCell(const GDouble* cellCenter, const GDouble cellLength) const -> GBool = 0;
  [[nodiscard]] virtual inline auto noObjects() const -> GInt                                                       = 0;
  [[nodiscard]] virtual inline auto getBoundingBox() const -> std::vector<GDouble>                                  = 0;

 private:
  MPI_Comm m_comm;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometryRepresentation {
 public:
  GeometryRepresentation(const json& geom) : m_inside(config::opt_config_value(geom, "inside", true)){};
  GeometryRepresentation()                              = default;
  virtual ~GeometryRepresentation()                     = default;
  GeometryRepresentation(const GeometryRepresentation&) = delete;
  GeometryRepresentation(GeometryRepresentation&&)      = delete;
  auto operator=(const GeometryRepresentation&) -> GeometryRepresentation& = delete;
  auto operator=(GeometryRepresentation&&) -> GeometryRepresentation& = delete;

  [[nodiscard]] virtual inline auto pointIsInside(const Point<NDIM>& x) const -> GBool                        = 0;
  [[nodiscard]] virtual inline auto cutWithCell(const Point<NDIM>& center, GDouble cellLength) const -> GBool = 0;
  [[nodiscard]] virtual inline auto getBoundingBox() const -> std::vector<GDouble>                            = 0;
  [[nodiscard]] virtual inline auto str() const -> GString                                                    = 0;

  [[nodiscard]] inline auto type() const -> GeomType { return m_type; }
  [[nodiscard]] inline auto name() const -> GString { return m_name; }
  [[nodiscard]] inline auto inside() const -> GBool { return m_inside; }

 protected:
  inline auto type() -> GeomType& { return m_type; }
  inline auto name() -> GString& { return m_name; }

 private:
  GeomType m_type   = GeomType::unknown;
  GString  m_name   = "undefined";
  GBool    m_inside = true;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometrySTL : public GeometryRepresentation<DEBUG_LEVEL, NDIM> {
 public:
  GeometrySTL(GString fileName, const GString& _name) : m_fileName(std::move(fileName)) {
    name() = _name;
    type() = GeomType::stl;
  };

  GeometrySTL(const json& stl, const GString& _name) : GeometryRepresentation<DEBUG_LEVEL, NDIM>(stl), m_fileName(stl["filename"]) {
    name() = _name;
    type() = GeomType::stl;
  };

  [[nodiscard]] auto inline pointIsInside(const Point<NDIM>& x) const -> GBool override { return false; }

  [[nodiscard]] inline auto cutWithCell(const Point<NDIM>& cellCenter, GDouble cellLength) const -> GBool override { return false; }

  [[nodiscard]] inline auto getBoundingBox() const -> std::vector<GDouble> override {
    std::vector<GDouble> bbox(2 * NDIM);
    std::fill(bbox.begin(), bbox.end(), 0);
    return bbox;
  }

  [[nodiscard]] inline auto str() const -> GString override {
    std::stringstream ss;
    ss << SP1 << "STL"
       << "\n";
    ss << SP7 << "Name: " << name() << "\n";
    ss << SP7 << "Filename: " << m_fileName << "\n";
    return ss.str();
  }

 private:
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::name;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::type;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::inside;

  GString m_fileName;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometryAnalytical : public GeometryRepresentation<DEBUG_LEVEL, NDIM> {
 public:
  GeometryAnalytical() = default;
  GeometryAnalytical(const json& geom) : GeometryRepresentation<DEBUG_LEVEL, NDIM>(geom){};

 private:
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeomSphere : public GeometryAnalytical<DEBUG_LEVEL, NDIM> {
 public:
  GeomSphere(const Point<NDIM>& center, GDouble radius, const GString& _name) : m_center(center), m_radius(radius) {
    name() = _name;
    type() = GeomType::sphere;
  };
  GeomSphere(const json& sphere, const GString& _name)
    : GeometryAnalytical<DEBUG_LEVEL, NDIM>(sphere),
      m_center(static_cast<std::vector<GDouble>>(sphere["center"]).data()),
      m_radius(sphere["radius"]) {
    name() = _name;
    type() = GeomType::sphere;
  };

  [[nodiscard]] auto inline pointIsInside(const Point<NDIM>& x) const -> GBool override {
    return (x - m_center).norm() < m_radius + GDoubleEps ? inside() : !inside();
  }

  [[nodiscard]] inline auto cutWithCell(const Point<NDIM>& cellCenter, GDouble cellLength) const -> GBool override {
    return (cellCenter - m_center).norm() < (m_radius + (gcem::sqrt(NDIM * gcem::pow(0.5 * cellLength, 2))) + GDoubleEps);
  }

  [[nodiscard]] inline auto getBoundingBox() const -> std::vector<GDouble> override {
    std::vector<GDouble> bbox(2 * NDIM);
    std::fill(bbox.begin(), bbox.end(), 0);
    for(GInt dir = 0; dir < NDIM; ++dir) {
      bbox[2 * dir]     = m_center[dir] - m_radius;
      bbox[2 * dir + 1] = m_center[dir] + m_radius;
    }
    return bbox;
  }

  [[nodiscard]] inline auto str() const -> GString override {
    std::stringstream ss;
    ss << SP1 << "Sphere"
       << "\n";
    ss << SP7 << "Name: " << name() << "\n";
    ss << SP7 << "Center: " << strStreamify<NDIM>(m_center).str() << "\n";
    ss << SP7 << "Radius: " << m_radius << "\n";
    return ss.str();
  }

 private:
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::name;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::type;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::inside;

  Point<NDIM> m_center{NAN};
  GDouble     m_radius = 0;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeomBox : public GeometryAnalytical<DEBUG_LEVEL, NDIM> {
 public:
  GeomBox(const Point<NDIM>& A, const Point<NDIM>& B, const GString& _name) : m_A(A), m_B(B) {
    name() = _name;
    type() = GeomType::box;
    checkValid();
  }
  GeomBox(const json& box, const GString& _name)
    : GeometryAnalytical<DEBUG_LEVEL, NDIM>(box),
      m_A(static_cast<std::vector<GDouble>>(box["A"]).data()),
      m_B(static_cast<std::vector<GDouble>>(box["B"]).data()) {
    name() = _name;
    type() = GeomType::box;
    checkValid();
  }

  [[nodiscard]] auto inline pointIsInside(const Point<NDIM>& x) const -> GBool override {
    for(GInt dir = 0; dir < NDIM; ++dir) {
      if(m_A[dir] > x[dir] || m_B[dir] < x[dir]) {
        return !inside();
      }
    }
    return inside();
  }

  [[nodiscard]] inline auto cutWithCell(const Point<NDIM>& cellCenter, GDouble cellLength) const -> GBool override {
    for(GInt dir = 0; dir < NDIM; ++dir) {
      if((m_A[dir] > cellCenter[dir] || m_B[dir] < cellCenter[dir]) // cellCenter is within the box
         && abs(m_A[dir] - cellCenter[dir]) > cellLength && abs(m_B[dir] - cellCenter[dir]) > cellLength /*cellcenter cuts the box*/) {
        return false;
      }
    }
    return true;
  }

  [[nodiscard]] inline auto getBoundingBox() const -> std::vector<GDouble> override {
    std::vector<GDouble> bbox(2 * NDIM);
    std::fill(bbox.begin(), bbox.end(), 0);
    for(GInt dir = 0; dir < NDIM; ++dir) {
      bbox[2 * dir]     = m_A[dir];
      bbox[2 * dir + 1] = m_B[dir];
    }
    return bbox;
  }

  [[nodiscard]] inline auto str() const -> GString override {
    std::stringstream ss;
    ss << SP1 << "Box"
       << "\n";
    ss << SP7 << "Name: " << name() << "\n";
    ss << SP7 << "Point A: " << strStreamify<NDIM>(m_A).str() << "\n";
    ss << SP7 << "Point B: " << strStreamify<NDIM>(m_B).str() << "\n";
    return ss.str();
  }

 private:
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::name;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::type;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::inside;


  void checkValid() const {
    for(GInt dir = 0; dir < NDIM; dir++) {
      if(m_A[dir] > m_B[dir]) {
        TERMM(-1, "ERROR: The specification of the box is invalid " + std::to_string(m_A[dir]) + " > " + std::to_string(m_B[dir]));
      }
    }
  }
  Point<NDIM> m_A;
  Point<NDIM> m_B;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeomCube : public GeometryAnalytical<DEBUG_LEVEL, NDIM> {
 public:
  GeomCube(const Point<NDIM>& center, const GDouble length, const GString& _name) : m_center(center), m_length(length) {
    name() = _name;
    type() = GeomType::cube;
  };
  GeomCube(const json& cube, const GString& _name)
    : GeometryAnalytical<DEBUG_LEVEL, NDIM>(cube),
      m_center(static_cast<std::vector<GDouble>>(cube["center"]).data()),
      m_length(cube["length"]) {
    name() = _name;
    type() = GeomType::cube;
  };

  [[nodiscard]] auto inline pointIsInside(const Point<NDIM>& x) const -> GBool override {
    for(GInt dir = 0; dir < NDIM; ++dir) {
      if(abs(x[dir] - m_center[dir]) > m_length) {
        return !inside();
      }
    }
    return inside();
  }

  [[nodiscard]] inline auto cutWithCell(const Point<NDIM>& cellCenter, GDouble cellLength) const -> GBool override {
    for(GInt dir = 0; dir < NDIM; ++dir) {
      if(abs(cellCenter[dir] - m_center[dir]) > m_length + cellLength) {
        return false;
      }
    }
    return true;
  }

  [[nodiscard]] inline auto getBoundingBox() const -> std::vector<GDouble> override {
    std::vector<GDouble> bbox(2 * NDIM);
    std::fill(bbox.begin(), bbox.end(), 0);
    GDouble cicumference_radius = gcem::sqrt(NDIM) * m_length;
    for(GInt dir = 0; dir < NDIM; ++dir) {
      bbox[2 * dir]     = m_center[dir] - cicumference_radius;
      bbox[2 * dir + 1] = m_center[dir] + cicumference_radius;
    }
    return bbox;
  }

  [[nodiscard]] inline auto str() const -> GString override {
    std::stringstream ss;
    ss << SP1 << "Cube"
       << "\n";
    ss << SP7 << "Name: " << name() << "\n";
    ss << SP7 << "Center: " << strStreamify<NDIM>(m_center).str() << "\n";
    ss << SP7 << "Length: " << m_length << "\n";
    return ss.str();
  }


 private:
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::name;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::type;
  using GeometryRepresentation<DEBUG_LEVEL, NDIM>::inside;


  Point<NDIM> m_center{NAN};
  GDouble     m_length = 0;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometryManager : public GeometryInterface {
 public:
  GeometryManager(const MPI_Comm comm) : GeometryInterface(comm){};

  void setup(const json& geometry) override {
    for(const auto& object : geometry.items()) {
      const GString& name = object.key();

      switch(resolveGeomType(geometry[name]["type"])) {
        case GeomType::sphere: {
          m_geomObj.emplace_back(std::make_unique<GeomSphere<DEBUG_LEVEL, NDIM>>(geometry[name], name));
          break;
        }
        case GeomType::box: {
          m_geomObj.emplace_back(std::make_unique<GeomBox<DEBUG_LEVEL, NDIM>>(geometry[name], name));
          break;
        }
        case GeomType::cube: {
          m_geomObj.emplace_back(std::make_unique<GeomCube<DEBUG_LEVEL, NDIM>>(geometry[name], name));
          break;
        }
        case GeomType::stl: {
          m_geomObj.template emplace_back(std::make_unique<GeometrySTL<DEBUG_LEVEL, NDIM>>(geometry[name], name));
          break;
        }
        case GeomType::unknown:
          [[fallthrough]];
        default: {
          logger << SP2 << "Unknown geometry type " << object.key() << std::endl;
          break;
        }
      }
      logger << m_geomObj.back()->str() << std::endl;
    }
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

  [[nodiscard]] auto inline noObjects() const -> GInt override { return m_geomObj.size(); }

  [[nodiscard]] inline auto getBoundingBox() const -> std::vector<GDouble> override {
    std::vector<GDouble> bbox(2 * NDIM);
    std::fill(bbox.begin(), bbox.end(), 0);

    for(GInt objId = 0; objId < static_cast<GInt>(m_geomObj.size()); ++objId) {
      const auto& obj       = m_geomObj.at(objId);
      const auto  temp_bbox = obj->getBoundingBox();
      for(GInt dir = 0; dir < NDIM; ++dir) {
        // set the bounding box to the values of the first object for initialization
        if(bbox[2 * dir] > temp_bbox[2 * dir] || objId == 0) {
          bbox[2 * dir] = temp_bbox[2 * dir];
        }
        if(bbox[2 * dir + 1] < temp_bbox[2 * dir + 1] || objId == 0) {
          bbox[2 * dir + 1] = temp_bbox[2 * dir + 1];
        }
      }
    }
    return bbox;
  }


 private:
  std::vector<std::unique_ptr<GeometryRepresentation<DEBUG_LEVEL, NDIM>>> m_geomObj;
};

#endif // GRIDGENERATOR_GEOMETRY_H
