#ifndef FLATMESHER_MESH_H_
#define FLATMESHER_MESH_H_

#include <vector>

#include "IndexTriangle.h"
#include "Point3.h"

namespace flat {

class Mesh {
public:
  Mesh() = default;
  Mesh(const Mesh&) = default;
  virtual ~Mesh() = default;

  std::vector<Point3> getNodes() const { return m_nodes; }
  const Point3& getNodeAt(size_t index) const { return m_nodes.at(index); }
  Point3& getNodeAt(size_t index) { return m_nodes.at(index); }
  std::vector<IndexTriangle> getMesh(size_t index_offset = 0) const;

  void setMesh(const std::vector<Point3>& nodes, const std::vector<IndexTriangle> edges);

  void move(double x, double y = 0.0, double z = 0.0);
  void invert();
  size_t addNode(const Point3& node);
  void addTriangle(const IndexTriangle& triangle);

  Mesh& operator=(const Mesh&) = default;

protected:
  std::vector<Point3> m_nodes;
  std::vector<IndexTriangle> m_mesh;

};

} // namespace flat

#endif // FLATMESHER_MESH_H_
