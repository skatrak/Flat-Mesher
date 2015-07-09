#include "FlatMesher/FloorPlan.h"
#include "FlatMesher/Utils.h"

#include <cmath>

using namespace flat;

bool FloorPlan::valid() const {
  // Check if the number of nodes is enough
  size_t sz_nodes = m_nodes.size();
  if (sz_nodes < 3)
    return false;

  // Check if the height is divisible by the triangle size
  double tmp = m_height / m_triangle_sz;
  if (!utils::isInteger(tmp))
    return false;

  // Check if sizes between points are divisible by the triangle size
  // and if points are given in CCW order
  double total = 0;
  for (size_t i = 1; i <= sz_nodes; ++i) {
    Point2 a = m_nodes[i - 1];
    Point2 b = m_nodes[i % sz_nodes];

    if (!utils::isInteger(a.distance(b)))
      return false;

    total += (b.getX() - a.getX()) * (a.getY() + b.getY());
  }

  // Points are not expressed in CCW order
  if (total >= 0.0)
    return false;

  // Check if there are no repeated points
  for (size_t i = 0; i < sz_nodes - 1; ++i)
    for (size_t j = i + 1; j < sz_nodes; ++j)
      if (m_nodes[i] == m_nodes[j])
        return false;

  return true;
}

std::ostream& operator<<(std::ostream& os, const FloorPlan& fp) {
  std::vector<Point2> nodes = fp.getNodes();

  os << nodes.size() << '\n';
  for (auto i = nodes.begin(); i != nodes.end(); ++i)
    os << *i << '\n';

  os << fp.getHeight() << '\n';
  os << fp.getTriangleSize() << '\n';
  
  return os;
}

std::istream& operator>>(std::istream& is, FloorPlan& fp) {
  size_t sz;
  is >> sz;

  std::vector<Point2> nodes(sz);
  for (auto i = nodes.begin(); i != nodes.end(); ++i)
    is >> *i;

  double height, tr_sz;
  is >> height;
  is >> tr_sz;

  fp.setNodes(nodes);
  fp.setHeight(height);
  fp.setTriangleSize(tr_sz);

  return is;
}
