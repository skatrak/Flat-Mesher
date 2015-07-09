#include "FlatMesher/FlatMesh.h"
#include "FlatMesher/FloorPlan.h"
#include "FlatMesher/Point2.h"
#include "FlatMesher/Rectangle.h"
#include "FlatMesher/WallMesh.h"

#include <cmath>

using namespace flat;

void FlatMesh::createFromPlan(const FloorPlan* plan) {
  if (plan == NULL || !plan->valid())
    return;

  m_plan = plan;

  std::vector<Point2> plan_nodes = plan->getNodes();
  size_t plan_sz = plan_nodes.size();

  // Create a vector with the accumulated number of nodes that the 3D meshes
  // will have, so that their creation can be done in parallel
  size_t* acc_nodes = new size_t[plan_sz];
  size_t nodes_y = plan->getHeight() / plan->getTriangleSize();

  acc_nodes[0] = 0;
  for (size_t i = 1; i <= plan_sz; ++i) {
    Point2 a = plan_nodes[i - 1];
    Point2 b = plan_nodes[i % plan_sz];

    size_t nodes_x = (size_t) (a.distance(b) / plan->getTriangleSize());
    size_t wall_nodes = (nodes_y + 1) * (nodes_x);

    acc_nodes[i] = acc_nodes[i - 1] + wall_nodes;
  }

  std::vector<WallMesh> walls;

  #pragma omp parallel for firstprivate(plan_sz, acc_nodes) shared(walls)
  for (size_t i = 1; i <= plan_sz; ++i) {
    Point2 a = plan_nodes[i - 1];
    Point2 b = plan_nodes[i % plan_sz];

    WallMesh wall = createWall(a, b, acc_nodes[i]);

    #pragma omp critical
    walls.push_back(wall);
  }

  Rectangle bounding_box = plan->boundingBox();
  FlatMesh ceiling = createCeiling(bounding_box, plan->getHeight());

  merge(walls, ceiling);

  delete[] acc_nodes;
}

WallMesh FlatMesh::createWall(const Point2& p1, const Point2& p2, int starting_index) const {

}

FlatMesh FlatMesh::createCeiling(Rectangle bounding_box, double height) const {

}

void FlatMesh::merge(std::vector<WallMesh> walls, FlatMesh ceiling) {

}

void FlatMesh::setFloorPlan(const FloorPlan* plan) {

}
