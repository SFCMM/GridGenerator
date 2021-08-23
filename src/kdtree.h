#ifndef GRIDGENERATOR_KDTREE_H
#define GRIDGENERATOR_KDTREE_H
#include <stack>

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
 public:
  KDTree()              = default;
  ~KDTree()             = default;
  KDTree(const KDTree&) = delete;
  KDTree(KDTree&&)      = delete;
  auto operator=(const KDTree&) -> KDTree& = delete;
  auto operator=(KDTree&&) -> KDTree& = delete;

  void buildTree(GeometryManager<DEBUG_LEVEL, NDIM>& gm) {
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

  void retrieveNodes(GDouble* targetRegion, GInt& noNodes, GInt*& nodeList){};

  void splitTree(GInt noSubTrees){};

  GInt get_root() { return m_root; };

 private:
  void reset() {
    m_root = -1;
    m_nodes.clear();
    m_nodeList.clear();
  }


  GInt                          m_root = -1;
  std::vector<KDNode>           m_nodes;
  std::array<GDouble, 2 * NDIM> m_minMax{};
  std::vector<GInt>             m_nodeList;
};
#endif // GRIDGENERATOR_KDTREE_H
