#ifndef GRIDGENERATOR_KDTREE_H
#define GRIDGENERATOR_KDTREE_H
#include <stack>
#include "triangle.h"

struct KDNode {
  GInt m_parent;
  GInt m_leftSubtree;
  GInt m_rightSubtree;
  GInt m_depth;

  GDouble m_pivot;

  GDouble m_min;
  GDouble m_max;

  /// Holds the id of the connected element
  GInt m_element;
};


/** This class implements an alternating digital tree (min/max KDTree)
 *
 *  The KDTree stores geometrical elements in a hierarchical 2-d dimensional
 *  order, allowing fast proximity queries.
 *  This implementation follows the structure outlined by Aftosmis:
 *
 *  von Karman Institute for Fluid Dynamics
 *  Lecture Series 1997-02
 *  28th computational fluid dynamics
 *  Solution Adaptive Cartesian Grid Methods for Aerodynamic Flows
 *  with Complex Geometries
 */
template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class GeometryManager;

template <Debug_Level DEBUG_LEVEL, GInt NDIM>
class KDTree {
  using Point = VectorD<NDIM>;

 public:
  KDTree()              = default;
  ~KDTree()             = default;
  KDTree(const KDTree&) = delete;
  KDTree(KDTree&&)      = delete;
  auto operator=(const KDTree&) -> KDTree& = delete;
  auto operator=(KDTree&&) -> KDTree& = delete;

  /// Build a min/max kd tree using the provided GeometryManager.
  /// \param gm Geometry Manager for which to build the kd tree
  void buildTree(GeometryManager<DEBUG_LEVEL, NDIM>& gm) {
    // todo: this is mostly the same as below find a way to unify

    // count number of total geometry elements
    // analytical geometries have 1
    // STLs 1 for each triangle
    const GInt noNodes = gm.noElements();

    // allocate memory
    m_nodes.resize(noNodes);
    logger << "Building KDTree tree with " << noNodes << " nodes " << std::endl;

    struct OffsetType {
      GInt from;
      GInt to;
    };

    std::vector<OffsetType> offset;
    offset.resize(noNodes);

    std::vector<GDouble> bbox = gm.getBoundingBox();

    // set root node
    m_root                   = 0;
    offset[m_root].from      = 0;
    offset[m_root].to        = noNodes - 1;
    m_nodes[m_root].m_parent = 0; // no parent
    m_nodes[m_root].m_depth  = 0;

    std::copy_n(bbox.begin(), 2 * NDIM, m_boundingBox.begin());

    if(noNodes == 1) {
      // with one node we don't need to search so we just return the root element anyway
      m_nodes[m_root].m_leftSubtree  = 0;
      m_nodes[m_root].m_rightSubtree = 0;
      m_nodes[m_root].m_element      = 0;

      // set some reasonable values
      m_nodes[m_root].m_max = bbox[1];
      m_nodes[m_root].m_min = bbox[0];
      return;
    }

    std::stack<GInt> nodeStack;
    nodeStack.push(m_root); // start with root

    std::vector<GInt> index(noNodes);
    // fill list with range from 0 to noNodes - 1
    std::iota(index.begin(), index.end(), 0);

    GInt nodeId = 0; // counter
    GInt left   = 0;
    GInt right  = 0;

    // start filling the tree
    while(!nodeStack.empty()) {
      const GInt currentNode = nodeStack.top();
      nodeStack.pop();
      const GInt currentDepth  = m_nodes[currentNode].m_depth;
      const GInt currentDir    = currentDepth % (NDIM * 2);
      auto&      currentOffset = offset[currentNode];
      const GInt offsetWidth   = currentOffset.to - currentOffset.from;
      cerr0 << "offsetWidth (GM)" << offsetWidth << std::endl; // todo: remove

      if(offsetWidth > 0) {
        left                               = ++nodeId;
        m_nodes[currentNode].m_leftSubtree = left;

        m_nodes[left].m_parent = currentNode;
        m_nodes[left].m_depth  = currentDepth + 1;
        nodeStack.push(left);

        if(offsetWidth > 1) {
          right                               = ++nodeId;
          m_nodes[currentNode].m_rightSubtree = right;

          m_nodes[right].m_parent = currentNode;
          m_nodes[right].m_depth  = currentDepth + 1;
          nodeStack.push(right);
        } else {
          // sub tree exhausted no right subtree
          right                               = 0;
          m_nodes[currentNode].m_rightSubtree = right;
        }
        // sort by the currentDir of the bounding boxes of each element
        // example: currentDir = 0 -> (min x dir) -3, -1, 2 , 4
        std::sort(index.begin() + currentOffset.from, index.begin() + currentOffset.to + 1,
                  [&](GInt a, GInt b) { return gm.elementBoundingBox(a, currentDir) < gm.elementBoundingBox(b, currentDir); });

        // remove own element from offset range
        m_nodes[currentNode].m_element = index[currentOffset.from];
        ++currentOffset.from;

        // find the pivot
        const GInt pivotElement      = (offsetWidth - 1) / 2 + currentOffset.from;
        m_nodes[currentNode].m_pivot = gm.elementBoundingBox(index[pivotElement], currentDir);


        // Left subtree (lower values)
        offset[left].from = currentOffset.from;
        offset[left].to   = pivotElement;

        // Right subtree (higher values)
        if(right > 0) {
          offset[right].from = offset[left].to + 1;
          offset[right].to   = currentOffset.to;
        }
      } else {
        // last element in a subtree
        right                               = 0;
        left                                = 0;
        m_nodes[currentNode].m_leftSubtree  = left;
        m_nodes[currentNode].m_rightSubtree = right;

        // Connect id to leaf
        m_nodes[currentNode].m_element = index[currentOffset.to];
        m_nodes[currentNode].m_pivot   = gm.elementBoundingBox(m_nodes[currentNode].m_element, currentDir);
      }
      // set the minimum and maximum value of all children
      m_nodes[currentNode].m_min = gm.elementBoundingBox(index[currentOffset.from - 1], currentDir); // -1 because from has been increased
      m_nodes[currentNode].m_max = gm.elementBoundingBox(index[currentOffset.to], currentDir);
    }
  };

  /// Build a min/max kd tree using the provided triangles.
  /// \param gm Geometry Manager for which to build the kd tree
  void buildTree(std::vector<triangle<NDIM>>& triangles, const std::vector<GDouble>& bbox) {
    // todo: this is mostly the same as above find a way to unify

    // count number of total geometry elements
    // analytical geometries have 1
    // STLs 1 for each triangle
    const GInt noNodes = triangles.size();

    // allocate memory
    m_nodes.resize(noNodes);
    logger << "Building KDTree tree with " << noNodes << " nodes " << std::endl;

    struct OffsetType {
      GInt from;
      GInt to;
    };

    std::vector<OffsetType> offset;
    offset.resize(noNodes);

    // set root node
    m_root                   = 0;
    offset[m_root].from      = 0;
    offset[m_root].to        = noNodes - 1;
    m_nodes[m_root].m_parent = 0; // no parent
    m_nodes[m_root].m_depth  = 0;

    std::copy_n(bbox.begin(), 2 * NDIM, m_boundingBox.begin());

    if(noNodes == 1) {
      // with one node we don't need to search so we just return the root element anyway
      m_nodes[m_root].m_leftSubtree  = 0;
      m_nodes[m_root].m_rightSubtree = 0;
      m_nodes[m_root].m_element      = 0;

      // set some reasonable values
      m_nodes[m_root].m_max = bbox[1];
      m_nodes[m_root].m_min = bbox[0];
      return;
    }

    std::stack<GInt> nodeStack;
    nodeStack.push(m_root); // start with root

    std::vector<GInt> index(noNodes);
    // fill list with range from 0 to noNodes - 1
    std::iota(index.begin(), index.end(), 0);

    GInt nodeId = 0; // counter
    GInt left   = 0;
    GInt right  = 0;

    // start filling the tree
    while(!nodeStack.empty()) {
      const GInt currentNode = nodeStack.top();
      nodeStack.pop();
      const GInt currentDepth  = m_nodes[currentNode].m_depth;
      const GInt currentDir    = currentDepth % (NDIM * 2);
      auto&      currentOffset = offset[currentNode];
      const GInt offsetWidth   = currentOffset.to - currentOffset.from;
      cerr0 << "offsetWidth " << offsetWidth << std::endl; // todo: remove
      if(offsetWidth < 0) {
        cerr0 << "currentOffset.to " << currentOffset.to << std::endl;
        cerr0 << "currentOffset.from " << currentOffset.from << std::endl;
      }

      if(offsetWidth > 0) {
        left                               = ++nodeId;
        m_nodes[currentNode].m_leftSubtree = left;

        m_nodes[left].m_parent = currentNode;
        m_nodes[left].m_depth  = currentDepth + 1;
        nodeStack.push(left);

        if(offsetWidth > 1) {
          right                               = ++nodeId;
          m_nodes[currentNode].m_rightSubtree = right;

          m_nodes[right].m_parent = currentNode;
          m_nodes[right].m_depth  = currentDepth + 1;
          nodeStack.push(right);
        } else {
          // sub tree exhausted no right subtree
          right                               = 0;
          m_nodes[currentNode].m_rightSubtree = right;
        }
        // sort by the currentDir of the bounding boxes of each element
        // example: currentDir = 0 -> (min x dir) -3, -1, 2 , 4
        std::sort(index.begin() + currentOffset.from, index.begin() + currentOffset.to + 1, [&](GInt a, GInt b) {
          return triangle_::boundingBox(triangles[a], currentDir) < triangle_::boundingBox(triangles[b], currentDir);
        });

        // remove own element from offset range
        m_nodes[currentNode].m_element = index[currentOffset.from];
        ++currentOffset.from;

        // find the pivot
        const GInt  pivotElementId   = (offsetWidth - 1) / 2 + currentOffset.from;
        const auto& pivotElement     = triangles[index[pivotElementId]];
        m_nodes[currentNode].m_pivot = triangle_::boundingBox(pivotElement, currentDir);

        // Left subtree (lower values)
        offset[left].from = currentOffset.from;
        offset[left].to   = pivotElementId;
        cerr0 << "offset[left].from " << offset[left].from << std::endl; // todo: remove
        cerr0 << "offset[left].to " << offset[left].to << std::endl;
        ASSERT(offset[left].to >= offset[left].from, "Invalid offset");


        // Right subtree (higher values)
        if(right > 0) {
          offset[right].from = offset[left].to + 1;
          offset[right].to   = currentOffset.to;
          cerr0 << "offset[right].from " << offset[right].from << std::endl; // todo: remove
          cerr0 << "offset[right].to " << offset[right].to << std::endl;
          ASSERT(offset[right].to >= offset[right].from, "Invalid offset");
        }
      } else {
        // last element in a subtree
        right                               = 0;
        left                                = 0;
        m_nodes[currentNode].m_leftSubtree  = left;
        m_nodes[currentNode].m_rightSubtree = right;

        // Connect id to leaf
        m_nodes[currentNode].m_element = index[currentOffset.to];
        m_nodes[currentNode].m_pivot   = triangle_::boundingBox(triangles[m_nodes[currentNode].m_element], currentDir);
      }
      // set the minimum and maximum value of all children
      m_nodes[currentNode].m_min =
          triangle_::boundingBox(triangles[index[currentOffset.from - 1]], currentDir); // -1 because from has been increased
      m_nodes[currentNode].m_max = triangle_::boundingBox(triangles[index[currentOffset.to]], currentDir);
    }
  };

  /// Retrieve all nodes(elements) that intersect with a provided bounding box.
  /// \param targetRegion Bounding box of the target region.
  /// \param gm Reference to geometry manager instance
  /// \param nodeList Reference to a vector to store the nodes which intersect the target region.
  void retrieveNodes(GeometryManager<DEBUG_LEVEL, NDIM>& gm, const GDouble* targetRegion, std::vector<GInt>& nodeList) const {
    if(m_nodes.empty()) {
      logger << "WARNING: Called retrieveNodes() but nothing to do!" << std::endl;
      return;
    }

    std::array<GDouble, 2 * NDIM> min;
    std::array<GDouble, 2 * NDIM> max;
    for(GInt dir = 0; dir < NDIM; ++dir) {
      min[dir]        = m_boundingBox[dir];
      min[dir + NDIM] = targetRegion[dir];
      max[dir]        = targetRegion[dir + NDIM];
      max[dir + NDIM] = m_boundingBox[dir + NDIM];
    }
    // Init empty stack and start at first node
    GInt             root     = 0;
    GBool            finished = false;
    std::stack<GInt> subtreeStack;
    subtreeStack.push(root);


    // Infinite loop until complete tree is traversed
    while(!finished) {
      // Check if the current nodes element-bounding box intersects target region
      const GInt currentElementId = m_nodes[root].m_element;
      GBool      doesOverlap      = true;

      // whole element is within the target range!!!
      for(GInt i = 0; i < 2 * NDIM; i++) {
        const GDouble value = gm.elementBoundingBox(currentElementId, i);
        if(value > max[i] || value < min[i]) {
          doesOverlap = false;
          break;
        }
      }

      // Inside target domain => add current element to return list
      if(doesOverlap) {
        nodeList.push_back(currentElementId);
      }

      const GInt currentDir = m_nodes[root].m_depth % (2 * NDIM);

      // todo: this can be rolled into a function
      //  if right subtree is inside target domain push on stack
      const GInt right = m_nodes[root].m_rightSubtree;
      if(right > 0 && m_nodes[root].m_max >= min[currentDir] && m_nodes[root].m_pivot <= max[currentDir]) {
        subtreeStack.push(right);
      }

      // if left subtree is inside target domain set it as root
      const GInt left = m_nodes[root].m_leftSubtree;
      if(left > 0 && m_nodes[root].m_pivot >= min[currentDir] && m_nodes[root].m_min <= max[currentDir]) {
        root = left;
        continue;
      }
      if(subtreeStack.empty()) {
        finished = true;
        continue;
      }
      root = subtreeStack.top();
      subtreeStack.pop();
    }
  };

  /// Retrive all nodes(elements) that can intersect with a provided point x.
  /// \param Point Point for which to search possible intersections.
  /// \param nodeList Reference to a vector to store the nodes which intersect the target region.
  void retrieveNodes(const Point& x, std::vector<GInt>& nodeList) const {
    if(m_nodes.empty()) {
      logger << "WARNING: Called retrieveNodes() but nothing to do!" << std::endl;
      return;
    }

    // Init empty stack and start at first node
    GInt             root     = 0;
    GBool            finished = false;
    std::stack<GInt> subtreeStack;
    subtreeStack.push(root);


    // Infinite loop until complete tree is traversed
    while(!finished) {
      // Check if the current nodes element-bounding box intersects target region
      const GInt currentElementId = m_nodes[root].m_element;

      nodeList.emplace_back(currentElementId);

      // current direction of the KD tree
      const GInt currentDir = m_nodes[root].m_depth % (2 * NDIM);

      // if right subtree can have point with in its range -> push on stack
      const GInt right = m_nodes[root].m_rightSubtree;
      if(right > 0 && m_nodes[root].m_max >= x[currentDir] && m_nodes[root].m_pivot <= x[currentDir]) {
        subtreeStack.push(right);
      }

      // todo: this can be rolled into a function
      //  if left subtree is inside target domain set it as root
      const GInt left = m_nodes[root].m_leftSubtree;
      if(left > 0 && m_nodes[root].m_pivot >= x[currentDir] && m_nodes[root].m_min <= x[currentDir]) {
        root = left;
        continue;
      }

      if(subtreeStack.empty()) {
        finished = true;
        continue;
      }
      root = subtreeStack.top();
      subtreeStack.pop();
    }
  };

  [[nodiscard]] auto get_root() const -> GInt { return m_root; };

 private:
  void reset() {
    m_root = -1;
    m_nodes.clear();
    m_nodeList.clear();
  }


  GInt                          m_root = -1;
  std::vector<KDNode>           m_nodes;
  std::array<GDouble, 2 * NDIM> m_boundingBox{};
  std::vector<GInt>             m_nodeList;
};
#endif // GRIDGENERATOR_KDTREE_H
