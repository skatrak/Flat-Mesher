#ifndef FLATMESHER_FLATMESH_H_
#define FLATMESHER_FLATMESH_H_

#include <iostream>
#include <vector>

#include "Point3.h"

namespace flat {

class FloorPlan;
class Point2;
class Rectangle;
class WallMesh;

class FlatMesh {
public:
  FlatMesh(): m_plan(NULL) {}
  virtual ~FlatMesh() {}

  void createFromPlan(const FloorPlan* plan);

  std::vector<Point3> getNodes() const { return m_nodes; }
  std::vector<size_t> getMesh() const { return m_mesh; }

  bool empty() const { return m_plan == NULL; }

protected:
  virtual WallMesh createWall(const Point2& p1, const Point2& p2, int starting_index) const;
  virtual FlatMesh createCeiling(Rectangle bounding_box, double height) const;
  virtual void merge(std::vector<WallMesh> walls, FlatMesh ceiling);

private:
  void setFloorPlan(const FloorPlan* plan);

  const FloorPlan* m_plan;
  std::vector<Point3> m_nodes;
  std::vector<size_t> m_mesh;

};

} // namespace flat

std::ostream& operator<<(std::ostream& os, const flat::FlatMesh& mesh);
std::istream& operator>>(std::istream& is, flat::FlatMesh& mesh);

#endif // FLATMESHER_FLATMESH_H_
