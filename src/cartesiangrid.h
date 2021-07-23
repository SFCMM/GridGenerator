#ifndef GRIDGENERATOR_CARTESIANGRID_H
#define GRIDGENERATOR_CARTESIANGRID_H

#include <gcem.hpp>

#include "gtree.h"
#include "macros.h"

class GridInterface {
 public:
  GridInterface()                     = default;
  virtual ~GridInterface()            = default;
  GridInterface(const GridInterface&) = delete;
  GridInterface(GridInterface&&)      = delete;
  auto operator=(const GridInterface&) -> GridInterface& = delete;
  auto operator=(GridInterface&&) -> GridInterface& = delete;

  virtual void setBoundingBox(std::vector<GDouble> bbox) = 0;
  virtual void setCapacity(GInt capacity)                = 0;
  virtual void setMinLvl(const GInt minLvl)              = 0;

  [[nodiscard]] virtual auto cog() const -> std::vector<GDouble>          = 0;
  [[nodiscard]] virtual auto geomExtent() const -> std::vector<GDouble>   = 0;
  [[nodiscard]] virtual auto boundingBox() const -> std::vector<GDouble>  = 0;
  [[nodiscard]] virtual auto decisiveDirection() const -> GDouble         = 0;
  [[nodiscard]] virtual auto lengthOnLvl(const GInt lvl) const -> GDouble = 0;

 private:
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class BaseCartesianGrid : public GridInterface {
 public:
  BaseCartesianGrid()                         = default;
  ~BaseCartesianGrid() override               = default;
  BaseCartesianGrid(const BaseCartesianGrid&) = delete;
  BaseCartesianGrid(BaseCartesianGrid&&)      = delete;
  auto operator=(const BaseCartesianGrid&) -> BaseCartesianGrid& = delete;
  auto operator=(BaseCartesianGrid&&) -> BaseCartesianGrid& = delete;

  void setBoundingBox(std::vector<GDouble> bbox) override {
    if(bbox.size() != 2 * NDIM) {
      TERMM(-1, "Invalid boundary box definition.");
    }
    std::copy_n(bbox.begin(), 2 * NDIM, m_boundingBox.begin());
    for(GInt dir = 0; dir < NDIM; ++dir) {
      m_geometryExtents[dir] = m_boundingBox[2 * dir + 1] - m_boundingBox[2 * dir];
      // direction of largest extent will be = 0 if all extents are equal
      m_decisiveDirection = m_geometryExtents[dir] > m_geometryExtents[m_decisiveDirection] ? dir : m_decisiveDirection;
      m_centerOfGravity[dir] = m_boundingBox[2 * dir] + HALF * (m_boundingBox[2 * dir + 1] - m_boundingBox[2 * dir]);
    }
    m_lengthOnLevel[0] = (1.0 + 1.0 / gcem::pow(static_cast<GDouble>(BASE2), static_cast<GDouble>(MAX_LVL)))
                         * m_geometryExtents[m_decisiveDirection];
    for(GInt l = 1; l < MAX_LVL; l++) {
      m_lengthOnLevel.at(l) = HALF * m_lengthOnLevel.at(l - 1);
    }
  }
  void setMinLvl(const GInt minLvl) override { m_minLvl = minLvl; }

  [[nodiscard]] inline auto cog() const -> std::vector<GDouble> override {
    return std::vector<GDouble>(m_centerOfGravity.begin(), m_centerOfGravity.end());
  };
  [[nodiscard]] inline auto geomExtent() const -> std::vector<GDouble> override {
    return std::vector<GDouble>(m_geometryExtents.begin(), m_geometryExtents.end());
  };
  [[nodiscard]] inline auto boundingBox() const -> std::vector<GDouble> override {
    return std::vector<GDouble>(m_boundingBox.begin(), m_boundingBox.end());
  };
  [[nodiscard]] inline auto decisiveDirection() const -> GDouble override { return m_decisiveDirection; };
  [[nodiscard]] inline auto lengthOnLvl(const GInt lvl) const -> GDouble override {
    if(DEBUG_LEVEL >= Debug_Level::debug) {
      return m_lengthOnLevel.at(lvl);
    }
    return m_lengthOnLevel[lvl]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  };

 private:
  GInt m_minLvl = 1;

  // box containing the whole geometry
  std::array<GDouble, 2 * NDIM> m_boundingBox{NAN};
  // extent of the geometry
  std::array<GDouble, NDIM> m_geometryExtents{NAN};
  // center of gravity of the geometry
  std::array<GDouble, NDIM> m_centerOfGravity{NAN};
  // direction of largest extent
  GDouble m_decisiveDirection{};
  // length of the cells on each level basest on the largest extent
  std::array<GDouble, MAX_LVL> m_lengthOnLevel{NAN};
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class CartesianGrid : public BaseCartesianGrid<DEBUG_LEVEL, NDIM> {
 public:
  CartesianGrid()                     = default;
  ~CartesianGrid() override           = default;
  CartesianGrid(const CartesianGrid&) = delete;
  CartesianGrid(CartesianGrid&&)      = delete;
  auto operator=(const CartesianGrid&) -> CartesianGrid& = delete;
  auto operator=(CartesianGrid&&) -> CartesianGrid& = delete;

  void setCapacity(const GInt capacity) override {
    if(!m_tree.empty()) {
      TERMM(-1, "Invalid operation tree already allocated.");
    }
    m_tree.reset(capacity);
  }

 private:
  cartesian::Tree<DEBUG_LEVEL, NDIM> m_tree;
};

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class CartesianGridGen : public BaseCartesianGrid<DEBUG_LEVEL, NDIM> {
 public:
  CartesianGridGen()                        = default;
  ~CartesianGridGen() override              = default;
  CartesianGridGen(const CartesianGridGen&) = delete;
  CartesianGridGen(CartesianGridGen&&)      = delete;
  auto operator=(const CartesianGridGen&) -> CartesianGridGen& = delete;
  auto operator=(CartesianGridGen&&) -> CartesianGridGen& = delete;

  void setCapacity(const GInt capacity) override {
    if(!m_coordinates.empty()) {
      TERMM(-1, "Invalid operation tree already allocated.");
    }
    m_coordinates.reserve(NDIM * capacity);
    m_parentId.reserve(capacity);
    m_globalId.reserve(capacity);


    m_noChildIds.reserve(2 * capacity);
    m_nghbrIds.reserve(2 * NDIM * capacity);
    m_childIds.reserve(maxNoChildren<NDIM>() * capacity);
    m_rfnDistance.reserve(capacity);
  }

  void setMinLvl(const GInt minLvl) {
    m_levelOffsets.reserve(2 * minLvl);
    BaseCartesianGrid<DEBUG_LEVEL, NDIM>::setMinLvl(minLvl);
  }

 private:
  std::vector<GInt>    m_levelOffsets{};
  std::vector<GDouble> m_coordinates{};
  std::vector<GInt>    m_parentId{};
  std::vector<GInt>    m_globalId{};
  std::vector<GInt>    m_noChildIds{};
  std::vector<GInt>    m_nghbrIds{};
  std::vector<GInt>    m_childIds{};
  std::vector<GInt>    m_rfnDistance{};
};

#endif // GRIDGENERATOR_CARTESIANGRID_H
