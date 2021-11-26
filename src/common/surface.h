#ifndef LBM_SURFACE_H
#define LBM_SURFACE_H

#include "interface/grid_interface.h"

// 3D: Surface 2D: Line 1D: Point
class SurfaceInterface {
 public:
  [[nodiscard]] virtual auto getCellList() const -> const std::vector<GInt>& = 0;
  virtual void               setCellList(const std::vector<GInt>& cellList)  = 0;
  virtual void               addCell(const GInt cellId, const GInt dir)      = 0;

 private:
};

// 3D: Surface 2D: Line 1D: Point
template <GInt NDIM>
class Surface : public SurfaceInterface {
 public:
  Surface(GridInterfaceD<NDIM>* grid) : m_center(&grid->center(0)), m_grid(grid){};
  ~Surface() = default;

  // todo:fix
  //  Surface(const Surface& copy) : m_center(copy.m_center), m_grid(copy.m_grid) {}
  //  Surface(Surface&&)      = delete;
  //  auto operator=(const Surface&) -> Surface& = delete;
  //  auto operator=(Surface&&) -> Surface& = delete;


  [[nodiscard]] auto getCellList() const -> const std::vector<GInt>& override { return m_cellId; }

  void setCellList(const std::vector<GInt>& cellList) override {
    m_cellId.clear();
    std::copy(cellList.begin(), cellList.end(), std::back_inserter(m_cellId));
  }

  void addCell(const GInt cellId, const GInt dir) override {
    m_cellId.emplace_back(cellId);
    m_normal.emplace_back();
    m_normal.back().fill(0);
    m_normal.back()[dir / 2] = 2 * (dir % 2) - 1;
  }

  auto normal(const GInt surfCellId) const -> const VectorD<NDIM>& { return m_normal[surfCellId]; }

  [[nodiscard]] auto size() const -> GInt {
    ASSERT(!m_cellId.empty(), "Not inited!");
    return m_cellId.size();
  }

  auto center(const GInt cellId) const -> const VectorD<NDIM>& { return m_center[cellId]; }

  [[nodiscard]] auto cellLength(const GInt cellId) const -> GDouble {
    cerr0 << cellId << std::endl;
    cerr0 << m_grid << std::endl;
    cerr0 << "grid dim " << m_grid->dim() << std::endl;

    cerr0 << "level " << static_cast<GInt>(m_grid->level(cellId)) << std::endl;
    return m_grid->lengthOnLvl(m_grid->level(cellId));
  }

  auto grid() const -> GridInterfaceD<NDIM>* { return m_grid; }

 private:
  std::vector<GInt>          m_cellId;
  std::vector<VectorD<NDIM>> m_normal;
  VectorD<NDIM>*             m_center = nullptr;
  GridInterfaceD<NDIM>*      m_grid   = nullptr;
};

#endif // LBM_SURFACE_H