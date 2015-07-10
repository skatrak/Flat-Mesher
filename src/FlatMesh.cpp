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

  // Create a vector with the accumulated number of nodes that the 3D meshes
  // will have, so that their creation can be done in parallel
  size_t* acc_nodes = new size_t[plan_sz];
  size_t nodes_y = (size_t) (plan->getHeight() / plan->getTriangleSize());

  acc_nodes[0] = 0;
  for (int i = 1; i <= plan_sz; ++i) {
    Point2 a = plan_nodes[i - 1];
    Point2 b = plan_nodes[i % plan_sz];

    size_t nodes_x = (size_t) (a.distance(b) / plan->getTriangleSize());
    size_t wall_nodes = (nodes_y + 1) * (nodes_x);

    acc_nodes[i] = acc_nodes[i - 1] + wall_nodes;
  }

  std::vector<Mesh> walls;

  #pragma omp parallel for firstprivate(plan_sz, acc_nodes) shared(walls) schedule(dynamic)
  for (int i = 1; i <= plan_sz; ++i) {
    Point2 a = plan_nodes[i - 1];
    Point2 b = plan_nodes[i % plan_sz];

    Mesh wall = createWall(a, b, acc_nodes[i]);

    #pragma omp critical
    walls.push_back(wall);
  }

  Rectangle bounding_box = plan->boundingBox();
  Mesh ceiling = createCeiling(bounding_box, plan->getHeight());

  merge(walls, ceiling);

  delete[] acc_nodes;
}

Mesh FlatMesh::createWall(const Point2& a, const Point2& b, int starting_index) const {
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
  for (size_t i = 0; i < nodes_xy - 1; ++i)
    for (size_t j = 0; j < nodes_z; ++j)
      wall.addNode(Point3(i * delta_x, i * delta_y, j * delta));

  // Add the edges that compose the mesh
  // For this to always order the indices of the nodes in counter-clockwise order
  // the 2D points of the floor plan must be specified in that order also
  for (size_t i = 0; i < nodes_xy - 2; ++i) {
    for (size_t j = 0; j < nodes_z - 1; ++j) {
      size_t actual_idx = (i * nodes_z) + j;

      /*  /|
         / |
        o__| */
      IndexTriangle t1(actual_idx, actual_idx + nodes_z, actual_idx + nodes_z + 1);

      /* ____
         |  /
         | /
         o/  */
      IndexTriangle t2(actual_idx, actual_idx + nodes_z + 1, actual_idx + 1);

      wall.addTriangle(t1);
      wall.addTriangle(t2);
    }
  }

  return wall;
}

Mesh FlatMesh::createCeiling(Rectangle box, double height) const {
  // TODO
  return Mesh();
}

void FlatMesh::merge(std::vector<Mesh> walls, Mesh ceiling) {
  // TODO
}

std::ostream& operator<<(std::ostream& os, const flat::FlatMesh& mesh) {
  // TODO
  return os;
}

std::istream& operator>>(std::istream& is, flat::FlatMesh& mesh) {
  // TODO
  return is;
}
