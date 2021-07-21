#ifndef GRIDGENERATOR_GRIDCELL_H
#define GRIDGENERATOR_GRIDCELL_H
/// Grid cell Property Labels.
enum class GridCell {
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
constexpr std::underlying_type<GridCell>::type p(const GridCell property) {
  return static_cast<std::underlying_type<GridCell>::type>(property);
}

using BitsetType = std::bitset<p(GridCell::NumProperties)>;
} // namespace grid::cell

#endif // GRIDGENERATOR_GRIDCELL_H
