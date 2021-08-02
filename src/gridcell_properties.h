#ifndef GRIDGENERATOR_GRIDCELL_PROPERTIES_H
#define GRIDGENERATOR_GRIDCELL_PROPERTIES_H
//todo: merge both of these enum classes since the bits don't matter!

/// Grid cell properties. (Until 63 bits are used this does not increase memory!)
// bitsets are initialized to 0!
enum class GridCellProperties {
  IsPeriodic,
  IsHalo,
  IsWindow,
  IsToDelete,
  IsPartLvlAncestor, ///< cell is ancestor of partition level
  IsPartitionCell,   ///< cell is a partition cell
  WasNewlyCreated,   ///< cell was recently created
  WasCoarsened,      ///< cell was recently coarsened
  WasRefined,        ///< cell was recently refined
  // <<< add new properties here
  NumProperties
};

namespace grid::cell {
/// Converts property name to underlying integer value
constexpr auto p(const GridCellProperties property) -> std::underlying_type<GridCellProperties>::type {
  return static_cast<std::underlying_type<GridCellProperties>::type>(property);
}

using BitsetType = std::bitset<p(GridCellProperties::NumProperties)>;
} // namespace grid::cell

/// GridGenerator cell properties. (Until 63 bits are used this does not increase memory!)
// bitsets are initialized to 0!
enum class GridGenCellProperties {
  IsInside,
  IsBndry,
  IsPeriodic,
  IsWindow,
  IsHalo,
  IsRefine,
  TmpMarker,
  Del,
  // <<< add new properties here
  NumProperties
};

namespace gridgen::cell {
/// Converts property name to underlying integer value
constexpr auto p(const GridGenCellProperties property) -> std::underlying_type<GridGenCellProperties>::type {
  return static_cast<std::underlying_type<GridGenCellProperties>::type>(property);
}

using BitsetType = std::bitset<p(GridGenCellProperties::NumProperties)>;
} // namespace gridgen::cell

#endif // GRIDGENERATOR_GRIDCELL_PROPERTIES_H
