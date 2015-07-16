#include "FlatMesher/BemgenMeshFormatter.h"
#include "FlatMesher/Mesh.h"

#include <vector>

using namespace flat;

std::ostream& BemgenMeshFormatter::writeMesh(std::ostream& os, const Mesh& mesh) const {
  std::vector<Point3> nodes = mesh.getNodes();
  std::vector<IndexTriangle> triangles = mesh.getMesh();

  os << "3\n3\n\n";

  os << nodes.size() << '\n';
  for (auto i = nodes.begin(); i != nodes.end(); ++i)
    os << *i << '\n';

  os << '\n' << triangles.size();
  for (auto i = triangles.begin(); i != triangles.end(); ++i)
    os << '\n' << *i;

  return os;
}

std::istream& BemgenMeshFormatter::readMesh(std::istream& is, Mesh& mesh) const {
  size_t sp_dim, nodes_el;

  is >> sp_dim;
  is >> nodes_el;
  if (sp_dim != 3 || nodes_el != 3) {
    is.clear(std::ios::failbit);
    return is;
  }

  size_t total_nodes;
  is >> total_nodes;

  std::vector<Point3> nodes(total_nodes);
  for (size_t i = 0; i < total_nodes; ++i)
    is >> nodes[i];

  size_t total_edges;
  is >> total_edges;

  std::vector<IndexTriangle> edges(total_edges, IndexTriangle(0, 0, 0));
  for (size_t i = 0; i < total_edges; ++i)
    is >> edges[i];

  mesh.setMesh(nodes, edges);

  return is;
}
