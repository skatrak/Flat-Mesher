#ifndef FLATMESHER_FLOORPLAN_H_
#define FLATMESHER_FLOORPLAN_H_

#include <iostream>
#include <string>
#include <vector>

#include "Point2.h"

namespace flat {

class FloorPlan {
public:
  FloorPlan(): m_height(0.0), m_triangle_sz(0.0) {}

  std::vector<Point2> getNodes() const { return m_nodes; }
  Point2 getNodeAt(size_t pos) { return m_nodes.at(pos); }
  double getHeight() const { return m_height; }
  double getTriangleSize() const { return m_triangle_sz; }

  void setNodes(std::vector<Point2> nodes) { m_nodes = nodes; }
  void setHeight(double height) { m_height = height; }
  void setTriangleSize(double size) { m_triangle_sz = size; }

  bool valid() const;
  bool pointInside(const Point2& p) const;

private:
  std::vector<Point2> m_nodes;
  double m_height;
  double m_triangle_sz;

};

} // namespace flat

std::ostream& operator<<(std::ostream& os, const flat::FloorPlan& fp);
std::istream& operator>>(std::istream& is, flat::FloorPlan& fp);

#endif // FLATMESHER_FLOORPLAN_H_
