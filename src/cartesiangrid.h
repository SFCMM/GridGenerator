#ifndef GRIDGENERATOR_CARTESIANGRID_H
#define GRIDGENERATOR_CARTESIANGRID_H

#include <gcem.hpp>

#include "cartesian.h"
#include "geometry.h"
#include "globaltimers.h"
#include "gtree.h"
#include "IO.h"
#include "macros.h"
#include "math/hilbert.h"
#include "timer.h"

struct LevelOffsetType {
 public:
  GInt begin;
  GInt end;
};

inline auto levelSize(LevelOffsetType& level) -> GInt { return level.end - level.begin; }

template <GInt NDIM>
using Point = VectorD<NDIM>;

template <GInt NDIM>
struct /*alignas(64)*/ NeighborList {
  std::array<GInt, cartesian::maxNoNghbrs<NDIM>()> n{INVALID_CELLID};
};

template <GInt NDIM>
struct /*alignas(64)*/ ChildList {
  std::array<GInt, cartesian::maxNoChildren<NDIM>()> c{INVALID_CELLID};
};

class GridInterface {
 public:
  GridInterface()          = default;
  virtual ~GridInterface() = default;

  // deleted constructors not needed
  GridInterface(const GridInterface&) = delete;
  GridInterface(GridInterface&&)      = delete;
  auto operator=(const GridInterface&) -> GridInterface& = delete;
  auto operator=(GridInterface&&) -> GridInterface& = delete;

  ////Setter functions.

  /// Set the bounding box for the current grid.
  /// \param bbox Provide the bounding box in the following format {x1, x2, y1, y2...}
  virtual void setBoundingBox(std::vector<GDouble> bbox) = 0;

  /// Set the maximum number of cells that this grid can use. Can only be called once!
  /// \param capacity The capacity of this grid object to store cells.
  virtual void setCapacity(GInt capacity) = 0;

  /// Maximum possible level of the grid to store.
  /// \param maxLvl The maximum possible level the grid object can store.
  virtual void setMaxLvl(const GInt maxLvl) = 0;

  virtual void setGeometryManager(std::shared_ptr<GeometryInterface> geom) = 0;

  //// Getter functions.

  /// Get the center of gravity of the grid.
  /// \return Center of gravity of the grid.
  [[nodiscard]] virtual inline auto cog() const -> std::vector<GDouble> = 0;

  /// Length of the sides of the bounding box.
  /// \return Bounding box size.
  [[nodiscard]] virtual inline auto lengthOfBoundingBox() const -> std::vector<GDouble> = 0;

  /// The bounding box of the grid.
  /// \return Bounding box.
  [[nodiscard]] virtual inline auto boundingBox() const -> std::vector<GDouble> = 0;

  /// Direction of the largest length of the bounding box.
  /// \return Direction of largest length of the bounding box..
  [[nodiscard]] virtual inline auto largestDir() const -> GInt = 0;

  /// The length of the cell at a given level.
  /// \param lvl The level of the cell length.
  /// \return The length of a cell at the provided level.
  [[nodiscard]] virtual inline auto lengthOnLvl(const GInt lvl) const -> GDouble = 0;

  /// Get the partition level of the grid.
  /// \return Partition level.
  [[nodiscard]] virtual inline auto partitionLvl() const -> GInt = 0;

  /// Set the partition level of the grid.
  /// \return Partition level.
  virtual inline auto partitionLvl() -> GInt& = 0;

  /// The maximum level supported by this grid.
  /// \return The maximum level this grid can possibly have.
  [[nodiscard]] virtual inline auto maxLvl() const -> GInt = 0;

  /// Get the currently highest level present in the grid.
  /// \return Return currently highest level.
  [[nodiscard]] virtual inline auto currentHighestLvl() const -> GInt = 0;


  //// Grid Generation specific
  /// Create the grid that is used for partitioning. This grid has the level of the option provided in the grid
  /// configuration file. The grid up to this level is always produced on a single MPI rank.
  /// \param partioningLvl Level of the partitioing grid.
  virtual void createPartitioningGrid(const GInt partioningLvl) = 0;

  /// Uniformly refine the grid up to the provided level.
  /// \param uniformLvl Level of uniform refinement.
  virtual void uniformRefineGrid(const GInt uniformLvl) = 0;

  /// Refine the cells that have been marked for refinement.
  /// \param noCellsToRefine The number of cells that have been marked.
  virtual void refineMarkedCells(const GInt noCellsToRefine) = 0;


  // todo: add the filename as an argument
  ////IO
  /// Save the grid to a file.
  virtual void save() = 0;

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

  void setMaxLvl(const GInt maxLvl) override {
    gridgen_log << "set maximum grid level " << maxLvl << std::endl;
    m_maxLvl = maxLvl;
  }

  void setGeometryManager(std::shared_ptr<GeometryInterface> geom) override {
    // we cast this here since we don't want to cast all the time to get rid of NDIM...
    m_geometry = std::static_pointer_cast<GeometryManager<DEBUG_LEVEL, NDIM>>(geom);
  }

  [[nodiscard]] inline auto cog() const -> std::vector<GDouble> override {
    return std::vector<GDouble>(m_centerOfGravity.begin(), m_centerOfGravity.end());
  };
  [[nodiscard]] inline auto lengthOfBoundingBox() const -> std::vector<GDouble> override {
    return std::vector<GDouble>(m_geometryExtents.begin(), m_geometryExtents.end());
  };
  [[nodiscard]] inline auto boundingBox() const -> std::vector<GDouble> override {
    return std::vector<GDouble>(m_boundingBox.begin(), m_boundingBox.end());
  };
  [[nodiscard]] inline auto largestDir() const -> GInt override { return m_decisiveDirection; };
  [[nodiscard]] inline auto partitionLvl() const -> GInt override { return m_partitioningLvl; };
  inline auto               partitionLvl() -> GInt& override { return m_partitioningLvl; }
  [[nodiscard]] inline auto maxLvl() const -> GInt override { return m_maxLvl; };
  [[nodiscard]] inline auto lengthOnLvl(const GInt lvl) const -> GDouble override {
    if(DEBUG_LEVEL >= Debug_Level::debug) {
      return m_lengthOnLevel.at(lvl);
    }
    return m_lengthOnLevel[lvl]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  };
  [[nodiscard]] inline auto currentHighestLvl() const -> GInt override { return m_currentHighestLvl; }

 protected:
  inline void increaseCurrentHighestLvl() {
    ASSERT(m_currentHighestLvl <= m_maxLvl, "Level increased over maximum level!");
    ++m_currentHighestLvl;
  }

  inline auto geometry() { return m_geometry; }
  inline auto geometry() const { return m_geometry; }

 private:
  std::shared_ptr<GeometryManager<DEBUG_LEVEL, NDIM>> m_geometry;

  GInt m_currentHighestLvl = 0;
  GInt m_partitioningLvl   = 0;
  GInt m_maxLvl            = 0;

  // box containing the whole geometry
  std::array<GDouble, 2 * NDIM> m_boundingBox{NAN};
  // extent of the geometry
  std::array<GDouble, NDIM> m_geometryExtents{NAN};
  // m_center of gravity of the geometry
  std::array<GDouble, NDIM> m_centerOfGravity{NAN};
  // direction of largest extent
  GInt m_decisiveDirection{};
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
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::partitionLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::maxLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::lengthOnLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::cog;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::increaseCurrentHighestLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::currentHighestLvl;
  using BaseCartesianGrid<DEBUG_LEVEL, NDIM>::geometry;

  using PropertyBitsetType = gridgen::cell::BitsetType;
  using CellProperties     = GridGenCellProperties;
  using ChildListType      = std::array<GInt, cartesian::maxNoChildren<NDIM>()>;

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

  void setMaxLvl(const GInt _maxLvl) override {
    m_levelOffsets.resize(_maxLvl + 1);
    BaseCartesianGrid<DEBUG_LEVEL, NDIM>::setMaxLvl(_maxLvl);
  }

  void createPartitioningGrid(const GInt partitioningLvl) override {
    RECORD_TIMER_START(TimeKeeper[Timers::GridPart]);
    if(m_capacity < 1) {
      TERMM(-1, "Invalid grid capacity.");
    }

    partitionLvl() = partitioningLvl;
    gridgen_log << SP1 << "Create partitioning grid with level " << partitionLvl() << std::endl;
    std::cout << SP1 << "Create partitioning grid with level " << partitionLvl() << std::endl;

    gridgen_log << SP2 << "+ initial cube length: " << lengthOnLvl(0) << std::endl;
    std::cout << SP2 << "+ initial cube length: " << lengthOnLvl(0) << std::endl;

    // make sure we have set some level...
    if(partitioningLvl > maxLvl()) {
      cerr0 << "WARNING: No maximum level set -> set to " << partitioningLvl << std::endl;
      gridgen_log << "WARNING: No maximum level set -> set to " << partitioningLvl << std::endl;
      setMaxLvl(partitioningLvl);
    }

    // use lazy initialization for grid generation and make sure final partitioning grid starts in the beginning
    if(isEven(partitionLvl())) {
      // initial cell placed in the beginning
      m_levelOffsets[0] = {0, 1};
      m_levelOffsets[1] = {m_capacity - cartesian::maxNoChildren<NDIM>(), m_capacity};
    } else {
      // initial cell placed at the end
      m_levelOffsets[0] = {m_capacity - 1, m_capacity};
      m_levelOffsets[1] = {0, cartesian::maxNoChildren<NDIM>()};
    }

    const GInt begin                         = m_levelOffsets[0].begin;
    m_center[begin]                          = Point<NDIM>(cog().data());
    m_globalId[begin]                        = begin;
    property(begin, CellProperties::IsBndry) = true;
    m_size                                   = 1;

    //  Refine to min level
    for(GInt l = 0; l < partitionLvl(); l++) {
      const GInt prevLevelBegin   = m_levelOffsets[l].begin;
      const GInt prevLevelEnd     = m_levelOffsets[l].end;
      const GInt prevLevelNoCells = prevLevelEnd - prevLevelBegin;
      if(DEBUG_LEVEL > Debug_Level::no_debug) {
        gridgen_log << "LevelOffset " << l << ":[" << prevLevelBegin << ", " << prevLevelEnd << "]" << std::endl;
      }


      if(m_levelOffsets[l].begin == 0) {
        // m_capacity - (prevLevelNoCells) * cartesian::maxNoChildren<NDIM>()
        // from the end - maximum number of cells at the current level
        const GInt newLevelBegin = m_capacity - (prevLevelNoCells)*cartesian::maxNoChildren<NDIM>();
        m_levelOffsets[l + 1]    = {newLevelBegin, m_capacity};

        if(prevLevelEnd > newLevelBegin) {
          outOfMemory(l + 1);
        }
      } else {
        const GInt newLevelEnd = (prevLevelNoCells)*cartesian::maxNoChildren<NDIM>();
        // from the start to the maximum number of cells at the current level
        m_levelOffsets.at(l + 1) = {0, newLevelEnd};

        if(prevLevelBegin < newLevelEnd) {
          outOfMemory(l + 1);
        }
      }

      refineGrid(m_levelOffsets, l);
      findChildLevelNghbrs(m_levelOffsets, l);
      deleteOutsideCells(l + 1);
      increaseCurrentHighestLvl();
    }

    std::fill(m_parentId.begin(), m_parentId.end(), INVALID_CELLID);
    reorderHilberCurve();

    RECORD_TIMER_STOP(TimeKeeper[Timers::GridPart]);
  }

  void uniformRefineGrid(const GInt uniformLevel) override {
    RECORD_TIMER_START(TimeKeeper[Timers::GridUniform]);
    gridgen_log << SP1 << "Uniformly refine grid to level " << uniformLevel << std::endl;
    std::cout << SP1 << "Uniformly refine grid to level " << uniformLevel << std::endl;

    if(partitionLvl() == uniformLevel) {
      return;
    }

    for(GInt l = partitionLvl(); l < uniformLevel; l++) {
      m_levelOffsets[l + 1] = {m_size, m_size + levelSize(m_levelOffsets[l]) * cartesian::maxNoChildren<NDIM>()};
      if(m_levelOffsets[l + 1].end > m_capacity) {
        outOfMemory(l + 1);
      }

      if(!MPI::isSerial()) {
        // updateHaloOffsets();
      }

      refineGrid(m_levelOffsets, l);
      if(!MPI::isSerial()) {
        // todo:implement
        // refineGrid(m_haloOffsets, l);
      }
      m_size = m_levelOffsets[l + 1].end;

      findChildLevelNghbrs(m_levelOffsets, l);
      if(!MPI::isSerial()) {
        // todo:implement
        // findChildLevelNeighbors(m_haloOffsets, l);
      }

      if(!MPI::isSerial()) {
        // todo:implement
        //        deleteOutsideCellsParallel(l + 1);
      } else {
        deleteOutsideCells(l + 1);
      }

      m_size = m_levelOffsets[l + 1].end;
      increaseCurrentHighestLvl();
    }
    RECORD_TIMER_STOP(TimeKeeper[Timers::GridUniform]);
  }

  void refineMarkedCells(const GInt noCellsToRefine) override {
    if(noCellsToRefine == 0) {
      return;
    }

    gridgen_log << SP1 << "Refining marked cells to level " << currentHighestLvl() + 1 << std::endl;
    std::cout << SP1 << "Refining marked cells to level " << currentHighestLvl() + 1 << std::endl;
    // update the offsets
    m_levelOffsets[currentHighestLvl() + 1] = {m_size, m_size + noCellsToRefine * cartesian::maxNoChildren<NDIM>()};
    if(m_levelOffsets[currentHighestLvl() + 1].end > m_capacity) {
      outOfMemory(currentHighestLvl() + 1);
    }
    gridgen_log << SP2 << "* cells to refine: " << noCellsToRefine << std::endl;
    std::cout << SP2 << "* cells to refine: " << noCellsToRefine << std::endl;


    if(!MPI::isSerial()) {
      // todo:implement
      // updateHaloOffsets();
    }

    // refine marked cells
    // todo:implement
    //    refineMarkedCells(m_currentHighestLvl);

    increaseCurrentHighestLvl();
  }

  void save() override {
    std::function<GBool(GInt)> isHighestLevel = [&](GInt cellId) {
      return std::to_integer<GInt>(m_level[cellId]) == currentHighestLvl();
    };

    std::function<GBool(GInt)> isLeaf = [&](GInt cellId) { return m_noChildren[cellId] == 0; };

    std::vector<GString>              index = {"Level", "NoChildren"};
    std::vector<std::vector<GString>> values;
    values.emplace_back(toStringVector(m_level, m_size));
    values.emplace_back(toStringVector(m_noChildren, m_size));


    ASCII::writePointsCSV<NDIM>("Test", m_size, m_center, index, values, isHighestLevel);
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

    ASSERT(level + 1 <= maxLvl(),
           "Invalid refinement level! " + std::to_string(level + 1) + ">" + std::to_string(maxLvl()));

    // refine all cells on the given level
    GInt cellCount = 0;
    for(GInt cellId = levelOffset[level].begin; cellId < levelOffset[level].end; ++cellId) {
      refineCell(cellId, levelOffset[level + 1].begin + cellCount * cartesian::maxNoChildren<NDIM>());
      ++cellCount;
    }
  }

  void refineCell(const GInt cellId, const GInt offset) {
    if(DEBUG_LEVEL > Debug_Level::debug) {
      gridgen_log << "refine cell " << cellId << " with offset " << offset << std::endl;
    }
    const GInt    refinedLvl       = std::to_integer<GInt>(m_level[cellId]) + 1;
    const GDouble refinedLvlLength = lengthOnLvl(refinedLvl);

    for(GInt childId = 0; childId < cartesian::maxNoChildren<NDIM>(); ++childId) {
      const GInt childCellId = offset + childId;
      // todo: replace childDir with constant expression function
      m_center[childCellId] =
          m_center[cellId] + HALF * Point<NDIM>(cartesian::childDir[childId].data()) * refinedLvlLength;
      m_level[childCellId]    = static_cast<std::byte>(refinedLvl);
      m_parentId[childCellId] = cellId;
      m_globalId[childCellId] = childCellId;

      // reset since we overwrite previous levels
      m_noChildren[childCellId] = 0;
      m_properties[childCellId].reset();
      m_childIds[childCellId] = {INVALID_CELLID};
      m_nghbrIds[childCellId] = {INVALID_CELLID};

      // if parent is a boundary cell check for children as well
      if(property(cellId, CellProperties::IsBndry)) {
        property(childCellId, CellProperties::IsBndry) = cellHasCut(childCellId);
      }

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
      const GInt* __restrict children = &m_childIds[parentId].c[0];
      for(GInt childId = 0; childId < cartesian::maxNoChildren<NDIM>(); ++childId) {
        if(children[childId] == INVALID_CELLID) {
          // no child
          continue;
        }

        // check all neighbors
        GInt* __restrict neighbors = &m_nghbrIds[children[childId]].n[0];
        for(GInt dir = 0; dir < cartesian::maxNoNghbrs<NDIM>(); ++dir) {
          // neighbor direction not set
          if(neighbors[dir] == INVALID_CELLID) {
            // todo: replace nghbrInside by const expression function
            const GInt nghbrId = cartesian::nghbrInside[childId][dir];
            // neighbor is within the same parent cell
            if(nghbrId != INVALID_CELLID) {
              neighbors[dir] = nghbrId;
            } else {
              // todo: replace nghbrParentChildId by const expression function
              const GInt parentLvlNeighborChildId = cartesian::nghbrParentChildId[childId][dir];
              ASSERT(parentLvlNeighborChildId > INVALID_CELLID, "The definition of nghbrParentChildId is wrong! "
                                                                "childId: "
                                                                    + std::to_string(childId) + " dir "
                                                                    + std::to_string(dir));

              const GInt parentLvlNghbrId = m_nghbrIds[parentId].n[dir];
              if(parentLvlNghbrId != INVALID_CELLID && parentLvlNeighborChildId != INVALID_CELLID
                 && m_childIds[parentLvlNghbrId].c[parentLvlNeighborChildId] != INVALID_CELLID) {
                neighbors[dir] = m_childIds[parentLvlNghbrId].c[parentLvlNeighborChildId];
              }
            }
          }
        }
      }
    }
  }

  void deleteOutsideCells(const GInt level) {
    markOutsideCells(m_levelOffsets, level);

    // delete cells that have been marked as being outside
    for(GInt cellId = m_levelOffsets[level].end - 1; cellId >= m_levelOffsets[level].begin; --cellId) {
      ASSERT(!property(cellId, CellProperties::IsBndry)
                 || property(cellId, CellProperties::IsInside) == property(cellId, CellProperties::IsBndry),
             "Properties not set correctly! IsBndry implies IsInside!");
      // remove cell since it is not inside
      if(!property(cellId, CellProperties::IsInside)) {
        const GInt parentId = m_parentId[cellId];
        ASSERT(parentId != INVALID_CELLID, "Invalid parentId!");

        // remove from parent
        updateParent(m_parentId[cellId], cellId, INVALID_CELLID);
        --m_noChildren[parentId];

        // remove from neighbors
        for(GInt dir = 0; dir < cartesian::maxNoNghbrs<NDIM>(); ++dir) {
          const GInt nghbrCellId = m_nghbrIds[cellId].n[dir];
          if(nghbrCellId != INVALID_CELLID) {
            m_nghbrIds[nghbrCellId].n[cartesian::oppositeDir(dir)] = INVALID_CELLID;
          }
        }
        if(cellId != m_levelOffsets[level].end - 1) {
          // copy an inside cell to the current position to fill the hole
          copyCell(m_levelOffsets[level].end - 1, cellId);
        }
        m_levelOffsets[level].end--;
      }
    }
    m_size = levelSize(m_levelOffsets[level]);
    gridgen_log << SP3 << "* grid has " << m_size << " cells" << std::endl;
    std::cout << SP3 << "* grid has " << m_size << " cells" << std::endl;
  }

  void markOutsideCells(const std::vector<LevelOffsetType>& levelOffset, const GInt level) {
    for(GInt cellId = levelOffset[level].begin; cellId < levelOffset[level].end; ++cellId) {
      property(cellId, CellProperties::TmpMarker) = false;
    }

    for(GInt cellId = levelOffset[level].begin; cellId < levelOffset[level].end; ++cellId) {
      if(property(cellId, CellProperties::TmpMarker)) {
        continue;
      }
      property(cellId, CellProperties::TmpMarker) = true;
      const GBool isBndryCell                     = property(cellId, CellProperties::IsBndry);
      property(cellId, CellProperties::IsInside)  = isBndryCell || pointIsInside(m_center[cellId]);
      if(!isBndryCell) {
        floodCells(cellId);
      }
    }
  }

  void floodCells(GInt cellId) {
    const GBool inside = property(cellId, CellProperties::IsInside);
    for(GInt id = 0; id < cartesian::maxNoNghbrs<NDIM>(); ++id) {
      const GInt nghbrId = m_nghbrIds[cellId].n[id];
      if(nghbrId != INVALID_CELLID && !property(nghbrId, CellProperties::TmpMarker)) {
        if(!property(nghbrId, CellProperties::IsBndry)) {
          property(nghbrId, CellProperties::IsInside) = inside;
          floodCells(nghbrId);
        } else {
          property(nghbrId, CellProperties::IsInside) = true;
        }
        property(nghbrId, CellProperties::TmpMarker) = true;
      }
    }
  }

  [[nodiscard]] auto pointIsInside(const Point<NDIM>& x) const -> GBool { return geometry()->pointIsInside(x); }

  [[nodiscard]] auto cellHasCut(GInt cellId) const -> GBool {
    const GDouble cellLength = lengthOnLvl(std::to_integer<GInt>(m_level[cellId]));
    return geometry()->cutWithCell(m_center[cellId], cellLength);
  }

  void copyCell(const GInt from, const GInt to) {
    ASSERT(!property(from, CellProperties::Del), "Invalid cell to be copied!");
    ASSERT(from >= 0, "Invalid from!");
    ASSERT(to >= 0, "Invalid to!");

    m_properties[to] = m_properties[from];
    m_level[to]      = m_level[from];
    m_center[to]     = m_center[from];
    m_globalId[to]   = m_globalId[from];
    m_parentId[to]   = m_parentId[from];
    m_nghbrIds[to]   = m_nghbrIds[from];
    m_childIds[to]   = m_childIds[from];
    m_noChildren[to] = m_noChildren[from];

    for(GInt dir = 0; dir < cartesian::maxNoNghbrs<NDIM>(); ++dir) {
      if(m_nghbrIds[to].n[dir] != INVALID_CELLID) {
        m_nghbrIds[m_nghbrIds[to].n[dir]].n[cartesian::oppositeDir(dir)] = to;
      }
    }

    if(m_parentId[to] != INVALID_CELLID) {
      updateParent(m_parentId[to], from, to);
    }

    for(GInt childId = 0; childId < cartesian::maxNoChildren<NDIM>(); ++childId) {
      if(m_childIds[to].c[childId] != INVALID_CELLID) {
        m_parentId[m_childIds[to].c[childId]] = to;
      }
    }
  }

  void swapCell(GInt idA, GInt idB) {
    copyCell(idA, m_capacity - 1);
    copyCell(idB, idA);
    copyCell(m_capacity - 1, idB);
  }

  void updateParent(const GInt parentId, const GInt oldChildCellId, const GInt newChildCellId) {
    ASSERT(parentId >= 0, "Invalid parentId!");
    for(GInt childId = 0; childId < cartesian::maxNoChildren<NDIM>(); ++childId) {
      if(m_childIds[parentId].c[childId] == oldChildCellId) {
        m_childIds[parentId].c[childId] = newChildCellId;
        return;
      }
    }
  }

  void reorderHilberCurve() {
    gridgen_log << SP2 << "+ reordering grid based on Hilbert curve" << std::endl;
    std::cout << SP2 << "+ reordering grid based on Hilbert curve" << std::endl;

    Point<NDIM>       centerOfGravity = Point<NDIM>(cog().data());
    std::vector<GInt> hilbertIds(m_size);
    std::vector<GInt> index(m_size);
    std::vector<GInt> pos(m_size);
    std::vector<GInt> rev(m_size);
    // generate indices
    std::iota(index.begin(), index.end(), 0);
    std::iota(pos.begin(), pos.end(), 0);
    std::iota(rev.begin(), rev.end(), 0);

    GInt hilbertLevel = partitionLvl();
    for(GInt cellId = 0; cellId < m_size; ++cellId) {
      // Normalization to unit cube
      // array() since there is no scalar addition for vectors...
      Point<NDIM> x      = ((m_center[cellId] - centerOfGravity).array() + HALF * lengthOnLvl(0)) / lengthOnLvl(0);
      hilbertIds[cellId] = hilbert::index<NDIM>(x, hilbertLevel);
    }
    if(DEBUG_LEVEL > Debug_Level::min_debug) {
      gridgen_log << "checking duplicated Hilbert Ids" << std::endl;
      std::vector<GInt> duplicatedIds = checkDuplicateIds(hilbertIds);
      if(!duplicatedIds.empty()) {
        for(auto id : duplicatedIds) {
          GInt cellId = 0;
          std::cerr << "duplicated id " << id << std::endl;
          for(auto hilbertId : hilbertIds) {
            if(id == hilbertId) {
              std::cerr << "cellId " << cellId << std::endl;
              std::cerr << m_center[cellId] << std::endl;
            }
            ++cellId;
          }
        }
        TERMM(-1, "Duplicated Hilbert Ids found!");
      }
    }

    // sort index by hilberId
    std::sort(index.begin(), index.end(), [&](int A, int B) -> bool { return hilbertIds[A] < hilbertIds[B]; });

    for(GInt id = 0; id < m_size; ++id) {
      const GInt hilbertPos = index[id];
      if(id != hilbertPos) {
        // is not already in correct position due to swapping
        if(id != pos[hilbertPos]) {
          swapCell(id, pos[hilbertPos]);
        }
        GInt tmp             = rev[id];
        rev[pos[hilbertPos]] = tmp;
        pos[tmp]             = pos[hilbertPos];
      }
    }

    if(DEBUG_LEVEL > Debug_Level::debug) {
      std::fill(hilbertIds.begin(), hilbertIds.end(), 0);
      for(GInt cellId = 0; cellId < m_size; ++cellId) {
        // Normalization to unit cube
        // array() since there is no scalar addition for vectors...
        Point<NDIM> x      = ((m_center[cellId] - centerOfGravity).array() + HALF * lengthOnLvl(0)) / lengthOnLvl(0);
        hilbertIds[cellId] = hilbert::index<NDIM>(x, hilbertLevel);
      }
      for(GInt cellId = 1; cellId < m_size; ++cellId) {
        if(hilbertIds[cellId - 1] > hilbertIds[cellId]) {
          std::cerr << hilbertIds[cellId - 1] << " > " << hilbertIds[cellId] << std::endl;
          TERMM(-1, "Hilbert Ids not ordered correctly!");
        }
      }
    }
  }
};

#endif // GRIDGENERATOR_CARTESIANGRID_H
