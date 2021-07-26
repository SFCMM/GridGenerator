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

inline auto levelSize(LevelOffsetType& level) -> GInt { return level.end - level.begin; }

// template <GInt NDIM>
// class Point {
//  public:
//   auto operator=(const std::vector<GDouble>& rhs) -> Point& {
//     for(int i = 0; i < NDIM; i++) {
//       m_coordinates[i] = rhs[i];
//     }
//     return *this;
//   }
//
//
//  private:
//   std::array<GDouble, NDIM> m_coordinates{NAN};
// };

template <GInt NDIM>
using Point = VectorD<NDIM>;

template <GInt NDIM>
struct NeighborList {
  std::array<GInt, maxNoNghbrs<NDIM>()> n{INVALID_CELLID};
};

template <GInt NDIM>
struct ChildList {
  std::array<GInt, maxNoChildren<NDIM>()> c{INVALID_CELLID};
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
  virtual void setMaxLvl(const GInt maxLvl)              = 0;

  [[nodiscard]] virtual inline auto cog() const -> std::vector<GDouble>          = 0;
  [[nodiscard]] virtual inline auto geomExtent() const -> std::vector<GDouble>   = 0;
  [[nodiscard]] virtual inline auto boundingBox() const -> std::vector<GDouble>  = 0;
  [[nodiscard]] virtual inline auto decisiveDirection() const -> GDouble         = 0;
  [[nodiscard]] virtual inline auto lengthOnLvl(const GInt lvl) const -> GDouble = 0;
  [[nodiscard]] virtual inline auto minLvl() const -> GInt                       = 0;
  [[nodiscard]] virtual inline auto maxLvl() const -> GInt                       = 0;
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
  void setMinLvl(const GInt minLvl) override {
    gridgen_log << "set minLVL " << minLvl << std::endl;
    m_minLvl = minLvl;
  }
  void setMaxLvl(const GInt maxLvl) override {
    gridgen_log << "set maxLVL " << maxLvl << std::endl;
    m_maxLvl = maxLvl;
  }

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
  [[nodiscard]] inline auto maxLvl() const -> GInt override { return m_maxLvl; };
  [[nodiscard]] inline auto lengthOnLvl(const GInt lvl) const -> GDouble override {
    if(DEBUG_LEVEL >= Debug_Level::debug) {
      return m_lengthOnLevel.at(lvl);
    }
    return m_lengthOnLevel[lvl]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  };

 private:
  GInt m_minLvl = 1;
  GInt m_maxLvl = 1;

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
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::maxLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::lengthOnLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::cog;

  using PropertyBitsetType = gridgen::cell::BitsetType;
  using CellProperties     = GridGenCellProperties;
  using ChildListType      = std::array<GInt, maxNoChildren<NDIM>()>;

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
    m_properties.resize(capacity);
    m_level.resize(capacity);
    m_capacity = capacity;
  }

  void setMinLvl(const GInt _minLvl) override {
    m_levelOffsets.resize(_minLvl);
    BaseCartesianGrid<DEBUG_LEVEL, NDIM>::setMinLvl(_minLvl);
  }
  void setMaxLvl(const GInt _maxLvl) override {
    m_levelOffsets.resize(_maxLvl);
    BaseCartesianGrid<DEBUG_LEVEL, NDIM>::setMaxLvl(_maxLvl);
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

    const GInt begin                         = m_levelOffsets[0].begin;
    m_center[begin]                          = Point<NDIM>(cog().data());
    m_globalId[begin]                        = begin;
    property(begin, CellProperties::IsBndry) = 1;
    m_size                                   = 1;

    //  Refine to min level
    for(GInt l = 0; l < minLvl(); l++) {
      const GInt prevLevelBegin   = m_levelOffsets[l].begin;
      const GInt prevLevelEnd     = m_levelOffsets[l].end;
      const GInt prevLevelNoCells = prevLevelEnd - prevLevelBegin;

      if(m_levelOffsets[l].begin == 0) {
        // m_capacity - (prevLevelNoCells) * maxNoChildren<NDIM>()
        // from the end - maximum number of cells at the current level
        const GInt newLevelBegin = m_capacity - (prevLevelNoCells)*maxNoChildren<NDIM>();
        m_levelOffsets[l + 1]    = {newLevelBegin, m_capacity};

        if(prevLevelEnd > newLevelBegin) {
          outOfMemory(l + 1);
        }
      } else {
        const GInt newLevelEnd = (prevLevelNoCells)*maxNoChildren<NDIM>();
        // from the start to the maximum number of cells at the current level
        m_levelOffsets[l + 1] = {0, newLevelEnd};

        if(prevLevelBegin < newLevelEnd) {
          outOfMemory(l + 1);
        }
      }

      refineGrid(m_levelOffsets, l);
      findChildLevelNghbrs(m_levelOffsets, l);
      // deleteOutsideCells(l+1);
    }


    RECORD_TIMER_STOP(TimeKeeper[Timers::GridMin]);
  }

  static constexpr auto memorySizePerCell() -> GInt {
    return sizeof(GInt) * (1 + 1 + 1 + 1 + 2) // m_parentId, m_globalId, m_noChildren, m_rfnDistance, m_levelOffsets
           + sizeof(Point<NDIM>)              // m_center
           + sizeof(NeighborList<NDIM>)       // m_nghbrIds
           + sizeof(ChildList<NDIM>)          // m_childIds
           + sizeof(PropertyBitsetType)       // m_properties
           + 1;                               // m_level
  }


 private:
  inline auto property(const GInt id, CellProperties p) -> auto { return m_properties[id][static_cast<GInt>(p)]; }
  [[nodiscard]] inline auto property(const GInt id, CellProperties p) const -> GBool {
    return m_properties[id][static_cast<GInt>(p)];
  }
  std::vector<LevelOffsetType>    m_levelOffsets{};
  std::vector<Point<NDIM>>        m_center{};
  std::vector<GInt>               m_parentId{INVALID_CELLID};
  std::vector<GInt>               m_globalId{INVALID_CELLID};
  std::vector<GInt>               m_noChildren{};
  std::vector<std::byte>          m_level{};
  std::vector<NeighborList<NDIM>> m_nghbrIds{};
  std::vector<ChildList<NDIM>>    m_childIds{};
  std::vector<GInt>               m_rfnDistance{};
  std::vector<PropertyBitsetType> m_properties{};

  GInt m_capacity{0};
  GInt m_size{0};

  void outOfMemory(GInt level) {
    cerr0 << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    gridgen_log << "ERROR: Not enough memory to generate grid! Increase maxNoCells: " << m_capacity << std::endl;
    cerr0 << "ERROR: Not enough memory to generate grid! Increase maxNoCells: " << m_capacity << std::endl;
    gridgen_log << "level " << level - 1 << " [" << m_levelOffsets[level - 1].begin << ", "
                << m_levelOffsets[level - 1].end << "]" << std::endl;
    cerr0 << "level " << level - 1 << " [" << m_levelOffsets[level - 1].begin << ", " << m_levelOffsets[level - 1].end
          << "]" << std::endl;
    gridgen_log << "level " << level << " [" << m_levelOffsets[level].begin << ", " << m_levelOffsets[level].end << "]"
                << std::endl;
    cerr0 << "level " << level << " [" << m_levelOffsets[level].begin << ", " << m_levelOffsets[level].end << "]"
          << std::endl;
    cerr0 << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

    TERMM(-1, "Out of memory!");
  }

  void refineGrid(const std::vector<LevelOffsetType>& levelOffset, const GInt level) {
    gridgen_log << SP2 << "+ refining grid on level: " << level << std::endl;
    std::cout << SP2 << "+ refining grid on level: " << level << std::endl;

    ASSERT(level + 1 < maxLvl(),
           "Invalid refinement level! " + std::to_string(level + 1) + "<" + std::to_string(maxLvl()));

    // refine all cells on the given level
    for(GInt cellId = levelOffset[level].begin; cellId < levelOffset[level].end; ++cellId) {
      refineCell(cellId, levelOffset[level + 1].begin);
    }
  }

  void refineCell(const GInt cellId, const GInt offset) {
    if(DEBUG_LEVEL > Debug_Level::debug) {
      gridgen_log << "refine cell " << cellId << " with offset " << offset << std::endl;
    }
    const GInt    refinedLvl       = std::to_integer<GInt>(m_level[cellId]) + 1;
    const GDouble refinedLvlLength = lengthOnLvl(refinedLvl);

    for(GInt childId = 0; childId < maxNoChildren<NDIM>(); ++childId) {
      const GInt childCellId = offset + childId;
      // todo: replace childDir with constant expression function
      m_center[childCellId]   = m_center[cellId] + HALF * Point<NDIM>(childDir[childId].data()) * refinedLvlLength;
      m_level[childCellId]    = static_cast<std::byte>(refinedLvl);
      m_parentId[childCellId] = cellId;
      m_globalId[childCellId] = childCellId;

      // reset since we overwrite previous levels
      m_noChildren[childCellId] = 0;
      m_properties[childCellId].reset();
      m_childIds[childCellId] = {INVALID_CELLID};
      m_nghbrIds[childCellId] = {INVALID_CELLID};

      // check for cuts with the geometry
      // todo: implement
      //      if(property(cellId, CellProperties::IsBndry)) {
      //        property(childCellId, CellProperties::IsBndry) = checkCellForCut(childCellId);
      //      }

      // update parent
      m_childIds[cellId].c[childId] = childCellId;
      m_noChildren[cellId]++;
    }
  }

  void findChildLevelNghbrs(const std::vector<LevelOffsetType>& levelOffset, const GInt level) {
    // check all children at the given level
    for(GInt parentId = levelOffset[level].begin; parentId < levelOffset[level].end; ++parentId) {
      const ChildListType& children = m_childIds[parentId].c;
      for(GInt childId = 0; childId < maxNoChildren<NDIM>(); ++childId) {
        if(children[childId] == INVALID_CELLID) {
          // no child
          continue;
        }
        // check all neighbors
        for(GInt dir = 0; dir < maxNoNghbrs<NDIM>(); ++dir) {
          // neighbor direction not set
          if(m_nghbrIds[children[childId]].n[dir] == INVALID_CELLID) {
            // todo: replace nghbrInside by const expression function
            const GInt nghbrId = nghbrInside[childId][dir];
            // neighbor is within the same parent cell
            if(nghbrId != INVALID_CELLID) {
              m_nghbrIds[children[childId]].n[dir] = nghbrId;
            } else {
              // todo: replace nghbrParentChildId by const expression function
              const GInt parentLvlNeighborChildId = nghbrParentChildId[childId][dir];
              ASSERT(parentLvlNeighborChildId > INVALID_CELLID, "The definition of nghbrParentChildId is wrong! "
                                                                "childId: "
                                                                    + std::to_string(childId) + " dir "
                                                                    + std::to_string(dir));

              const GInt parentLvlNghbrId = m_nghbrIds[parentId].n[dir];
              if(parentLvlNghbrId != INVALID_CELLID && parentLvlNeighborChildId != INVALID_CELLID
                 && m_childIds[parentLvlNghbrId].c[parentLvlNeighborChildId] != INVALID_CELLID) {
                m_nghbrIds[children[childId]].n[dir] = m_childIds[parentLvlNghbrId].c[parentLvlNeighborChildId];
              }
            }
          }
        }
      }
    }
  }
};

#endif // GRIDGENERATOR_CARTESIANGRID_H
