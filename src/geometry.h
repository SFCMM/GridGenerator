#ifndef GRIDGENERATOR_GEOMETRY_H
#define GRIDGENERATOR_GEOMETRY_H
class GeometryInterface {
 public:
  GeometryInterface(const MPI_Comm comm) : m_comm(comm){};

 private:
  MPI_Comm m_comm;
};

template <GInt NDIM>
class Geometry : public GeometryInterface {
 public:
  Geometry(const MPI_Comm comm) : GeometryInterface(comm){};

 private:
};

#endif // GRIDGENERATOR_GEOMETRY_H
