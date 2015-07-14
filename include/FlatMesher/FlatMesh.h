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
  Mesh createCeiling(const Rectangle& box, std::vector<size_t>& boundary_nodes) const;
  void merge(const std::vector<Mesh>& walls, const Mesh& ceiling);

  inline static void submesh(size_t a_idx, size_t b_idx, size_t c_idx, size_t d_idx,
                             bool a_in, bool b_in, bool c_in, bool d_in, Mesh& mesh);

private:
  const FloorPlan* m_plan;

};

} // namespace flat

std::ostream& operator<<(std::ostream& os, const flat::FlatMesh& mesh);
std::istream& operator>>(std::istream& is, flat::FlatMesh& mesh);

#endif // FLATMESHER_FLATMESH_H_
