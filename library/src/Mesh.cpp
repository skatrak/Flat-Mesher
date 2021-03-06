#include "FlatMesher/Mesh.h"

using namespace flat;

std::vector<IndexTriangle> Mesh::getMesh(size_t index_offset) const {
  if (index_offset == 0)
    return m_mesh;

  std::vector<IndexTriangle> copy;
  copy.reserve(m_mesh.size());

  for (auto i = m_mesh.begin(); i != m_mesh.end(); ++i)
    copy.push_back(i->offset(index_offset));

  return copy;
}

void Mesh::setMesh(const std::vector<Point3>& nodes, const std::vector<IndexTriangle> edges) {
  m_nodes = nodes;
  m_mesh = edges;
}

size_t Mesh::addNode(const Point3& node) {
  m_nodes.push_back(node);
  return m_nodes.size() - 1;
}

void Mesh::addTriangle(const IndexTriangle& triangle) {
  size_t sz = m_nodes.size();

  if (triangle.getI() < sz && triangle.getJ() < sz && triangle.getK() < sz)
    m_mesh.push_back(triangle);
}

void Mesh::move(double x, double y, double z) {
  Point3 translation(x, y, z);
  for (auto i = m_nodes.begin(); i != m_nodes.end(); ++i)
    *i = *i + translation;
}

void Mesh::invert() {
  for (auto i = m_mesh.begin(); i != m_mesh.end(); ++i)
    i->invertRotation();
}
