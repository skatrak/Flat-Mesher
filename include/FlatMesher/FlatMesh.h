#ifndef FLATMESHER_FLATMESH_H_
#define FLATMESHER_FLATMESH_H_

#include <iostream>
#include <vector>

#include "Mesh.h"
#include "Point3.h"

namespace flat {

class FloorPlan;
class Point2;
class Rectangle;

class FlatMesh: public Mesh {
public:
  FlatMesh(): m_plan(NULL) {}
  FlatMesh(const FlatMesh& mesh): Mesh(mesh), m_plan(mesh.m_plan) {}
  ~FlatMesh() = default;

  void createFromPlan(const FloorPlan* plan);
  bool empty() const { return m_plan == NULL; }

  FlatMesh& operator=(const FlatMesh&) = default;

protected:
  Mesh createWall(const Point2& a, const Point2& b) const;
  Mesh createCeiling(const Rectangle& box, double height) const;
  void merge(const std::vector<Mesh>& walls, const Mesh& ceiling);

private:
  const FloorPlan* m_plan;

};

} // namespace flat

std::ostream& operator<<(std::ostream& os, const flat::FlatMesh& mesh);
std::istream& operator>>(std::istream& is, flat::FlatMesh& mesh);

#endif // FLATMESHER_FLATMESH_H_
