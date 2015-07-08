#include "FlatMesher/FloorPlan.h"

using namespace flat;

bool FloorPlan::valid() const {
  // TODO Check if sizes between points and the height are divisible by the triangle size
  // TODO Check if there are no intersecting walls
  // TODO Check if there aren't repeated points
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
