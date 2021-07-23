#ifndef GRIDGENERATOR_CARTESIANGRID_H
#define GRIDGENERATOR_CARTESIANGRID_H

#include <gcem.hpp>

#include "globaltimers.h"
#include "gtree.h"
#include "macros.h"
#include "timer.h"

struct LevelOffsetType {
 public:
  GInt begin;
  GInt end;
};

template <GInt NDIM>
class Point {
 public:
  auto operator=(const std::vector<GDouble>& rhs) -> Point& {
    for(int i = 0; i < NDIM; i++) {
      m_coordinates[i] = rhs[i];
    }
    return *this;
  }

  // private:
  std::array<GDouble, NDIM> m_coordinates{NAN};
};

template <GInt NDIM>
struct NeighborList {
  std::array<GInt, maxNoNghbrs<NDIM>()> n{INVALID_CELLID};
};

template <GInt NDIM>
struct ChildList {
  std::array<GInt, maxNoChildren<NDIM>()> n{INVALID_CELLID};
};

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

  [[nodiscard]] virtual inline auto cog() const -> std::vector<GDouble>          = 0;
  [[nodiscard]] virtual inline auto geomExtent() const -> std::vector<GDouble>   = 0;
  [[nodiscard]] virtual inline auto boundingBox() const -> std::vector<GDouble>  = 0;
  [[nodiscard]] virtual inline auto decisiveDirection() const -> GDouble         = 0;
  [[nodiscard]] virtual inline auto lengthOnLvl(const GInt lvl) const -> GDouble = 0;
  [[nodiscard]] virtual inline auto minLvl() const -> GInt                       = 0;
  //  virtual auto                      levelOffset(GInt level) -> LevelOffsetType&  = 0;
  //  virtual auto                      center(const GInt id) -> GDouble&            = 0;
  //  virtual auto                      nghbrId(const GInt id) -> GInt&              = 0;

  //// Grid Generation specific
  virtual void createMinLvlGrid() = 0;

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
  [[nodiscard]] inline auto minLvl() const -> GInt override { return m_minLvl; };
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
  // m_center of gravity of the geometry
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
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::minLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::lengthOnLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::cog;

  CartesianGridGen()                        = default;
  ~CartesianGridGen() override              = default;
  CartesianGridGen(const CartesianGridGen&) = delete;
  CartesianGridGen(CartesianGridGen&&)      = delete;
  auto operator=(const CartesianGridGen&) -> CartesianGridGen& = delete;
  auto operator=(CartesianGridGen&&) -> CartesianGridGen& = delete;

  void setCapacity(const GInt capacity) override {
    if(!m_center.empty()) {
      TERMM(-1, "Invalid operation tree already allocated.");
    }
    m_center.resize(capacity);
    m_parentId.resize(capacity);
    m_globalId.resize(capacity);
    m_noChildren.resize(capacity);
    m_nghbrIds.resize(capacity);
    m_childIds.resize(capacity);
    m_rfnDistance.resize(capacity);
    m_capacity = capacity;
  }

  void setMinLvl(const GInt _minLvl) override {
    m_levelOffsets.reserve(_minLvl);
    BaseCartesianGrid<DEBUG_LEVEL, NDIM>::setMinLvl(_minLvl);
  }

  //  auto levelOffset(const GInt level) -> LevelOffsetType& override { return m_levelOffsets[level]; }
  //  auto center(const GInt id) -> GDouble& override { return &m_center[id]; }

  void createMinLvlGrid() override {
    RECORD_TIMER_START(TimeKeeper[Timers::GridMin]);
    if(m_capacity < 1) {
      TERMM(-1, "Invalid grid capacity.");
    }

    gridgen_log << SP1 << "(3) create minLevel grid with level " << minLvl() << std::endl;
    std::cout << SP1 << "(3) create minLevel grid with level " << minLvl() << std::endl;

    gridgen_log << SP2 << "+ initial cube length: " << lengthOnLvl(0) << std::endl;
    std::cout << SP2 << "+ initial cube length: " << lengthOnLvl(0) << std::endl;

    // use lazy initialization for grid generation and make sure final minLevel grid starts in the beginning
    if(isEven(minLvl())) {
      // initial cell placed in the beginning
      m_levelOffsets[0] = {0, 1};
      m_levelOffsets[1] = {m_capacity - maxNoChildren<NDIM>(), m_capacity};
    } else {
      // initial cell placed at the end
      m_levelOffsets[0] = {m_capacity - 1, m_capacity};
      m_levelOffsets[1] = {0, maxNoChildren<NDIM>()};
    }

    const GInt begin = m_levelOffsets[0].begin;
    m_center[begin]  = cog();

    RECORD_TIMER_STOP(TimeKeeper[Timers::GridMin]);
  }

  static constexpr auto memorySizePerCell() -> GInt {
    return sizeof(GInt) * (1 + 1 + 1 + 1 + 2) + sizeof(Point<NDIM>) + sizeof(NeighborList<NDIM>)
           + sizeof(ChildList<NDIM>);
  }


 private:
  std::vector<LevelOffsetType>    m_levelOffsets{};
  std::vector<Point<NDIM>>        m_center{};
  std::vector<GInt>               m_parentId{INVALID_CELLID};
  std::vector<GInt>               m_globalId{INVALID_CELLID};
  std::vector<GInt>               m_noChildren{};
  std::vector<NeighborList<NDIM>> m_nghbrIds{};
  std::vector<ChildList<NDIM>>    m_childIds{};
  std::vector<GInt>               m_rfnDistance{};

  GInt m_capacity{0};
};

#endif // GRIDGENERATOR_CARTESIANGRID_H
