#include "FlatMesher/FlatMesh.h"
#include "FlatMesher/FloorPlan.h"
#include "FlatMesher/Point2.h"
#include "FlatMesher/Rectangle.h"

#include <cmath>

using namespace flat;

void FlatMesh::createFromPlan(const FloorPlan* plan) {
  if (m_plan != NULL) {
    m_nodes.clear();
    m_mesh.clear();
  }

  if (plan == NULL || !plan->valid())
    return;

  m_plan = plan;

  std::vector<Point2> plan_nodes = plan->getNodes();
  int plan_sz = plan_nodes.size();
  std::vector<Mesh> walls(plan_sz);

  #pragma omp parallel default(none) firstprivate(plan_sz) shared(plan_nodes, walls, plan)
  {
    #pragma omp for schedule(dynamic) nowait
    for (int i = 1; i <= plan_sz; ++i) {
      Point2 a = plan_nodes[i - 1];
      Point2 b = plan_nodes[i % plan_sz];

      Mesh wall = createWall(a, b);

      // Thread-safe
      walls[i - 1] = wall;
    }

    Mesh ceiling;

    // TODO Which is faster? Copying unnecessarily the variable to every thread
    // or waiting for the loop to finish before executing this?

    // This can be executed before the for loop has finished
    #pragma omp single copyprivate(ceiling)
    ceiling = createCeiling(plan->boundingBox(), plan->getHeight());

    // This only executes once the previous block and the loop are finished
    #pragma omp single
    merge(walls, ceiling);
  }
}

Mesh FlatMesh::createWall(const Point2& a, const Point2& b) const {
  Mesh wall;

  double delta = m_plan->getTriangleSize();

  // z represents the height, and xy is the plane where the two points are
  size_t nodes_z = (size_t) (m_plan->getHeight() / delta);
  size_t nodes_xy = (size_t) (a.distance(b) / delta);

  // We use this to be able to iterate over the line that contains the two
  // points, even when it is not vertical or horizontal
  double delta_x = (b.getX() - a.getX()) / nodes_xy;
  double delta_y = (b.getY() - a.getY()) / nodes_xy;

  // Add the nodes in column order and from bottom to top
  // Leave out the last column, because it's created in the next wall
  // The edges between the last column and the second to last column are added by
  // the merge() method
  Point3 origin(a);
  for (size_t i = 0; i < nodes_xy; ++i)
    for (size_t j = 0; j <= nodes_z; ++j)
      wall.addNode(origin + Point3(i * delta_x, i * delta_y, j * delta));

  // Add the edges that compose the mesh
  // For this to always order the indices of the nodes in counter-clockwise order
  // the 2D points of the floor plan must be specified in that order also
  for (size_t i = 0; i < nodes_xy - 1; ++i) {
    for (size_t j = 0; j < nodes_z; ++j) {
      size_t actual_idx = (i * (nodes_z + 1)) + j;

      /* /|
        / |
       o__| */
      IndexTriangle t1(actual_idx, actual_idx + nodes_z + 1, actual_idx + nodes_z + 2);

      /* ____
         |  /
         | /
         o/  */
      IndexTriangle t2(actual_idx, actual_idx + nodes_z + 2, actual_idx + 1);

      wall.addTriangle(t1);
      wall.addTriangle(t2);
    }
  }

  return wall;
}

Mesh FlatMesh::createCeiling(const Rectangle& box, double height) const {
  // TODO
  // Maybe return a list of indices of nodes that are part of the boundaries, so
  // that the search for the nodes doesn't take long
  return Mesh();
}

void FlatMesh::merge(const std::vector<Mesh>& walls, const Mesh& ceiling) {
  // We merge the walls by offsetting the indices and creating the sub-mesh
  // of each corner that we didn't do before
  size_t acc_nodes = 0;
  size_t nodes_z = (size_t) (m_plan->getHeight() / m_plan->getTriangleSize()) + 1;
  size_t total_nodes = (size_t) m_plan->boundaryLength() * nodes_z;

  double boundary = m_plan->boundaryLength();
  for (size_t i = 0; i < walls.size(); ++i) {
    Mesh wall = walls[i];

    std::vector<Point3> nodes = wall.getNodes();
    std::vector<IndexTriangle> indices = wall.getMesh(acc_nodes);

    m_nodes.insert(m_nodes.end(), nodes.begin(), nodes.end());
    m_mesh.insert(m_mesh.end(), indices.begin(), indices.end());

    acc_nodes += nodes.size();

    for (size_t j = 0; j < nodes_z - 1; ++j) {
      size_t actual_idx = acc_nodes - nodes_z + j;

      IndexTriangle t1(actual_idx, (actual_idx + nodes_z) % total_nodes, (actual_idx + nodes_z + 1) % total_nodes);
      IndexTriangle t2(actual_idx, (actual_idx + nodes_z + 1) % total_nodes, actual_idx + 1);

      m_mesh.push_back(t1);
      m_mesh.push_back(t2);
    }
  }

  // TODO Create floor. Merge ceiling and floor with the rest of the mesh.
  // The indices coming from the ceiling have to be offset by "total_nodes"
  // Then it's needed to walk over every segment and find the corresponding
  // nodes in the ceiling to delete them and change the triangles where they appear
  // Maybe it's a good idea to create a translation table and then apply it
}

std::ostream& operator<<(std::ostream& os, const flat::FlatMesh& mesh) {
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

std::istream& operator>>(std::istream& is, flat::FlatMesh& mesh) {
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
