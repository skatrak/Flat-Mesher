#include "FlatMesher/FloorPlan.h"
#include "FlatMesher/Line2.h"
#include "FlatMesher/AbortPlanErrorChecker.h"
#include "FlatMesher/Utils.h"

#include <limits>
#include <cmath>

using namespace flat;

Rectangle FloorPlan::boundingBox() const {
  double min_x = std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_x = std::numeric_limits<double>::min();
  double max_y = std::numeric_limits<double>::min();

  for (auto i = m_nodes.begin(); i != m_nodes.end(); ++i) {
    double x = i->getX(), y = i->getY();

    if (x < min_x) min_x = x;
    if (y < min_y) min_y = y;
    if (x > max_x) max_x = x;
    if (y > max_y) max_y = y;
  }

  return Rectangle(max_y, min_y, min_x, max_x);
}

double FloorPlan::boundaryLength() const {
  size_t sz_nodes = m_nodes.size();
  double len = 0.0;

  for (size_t i = 0; i < sz_nodes; ++i) {
    Point2 a = m_nodes[i];
    Point2 b = m_nodes[(i + 1) % sz_nodes];

    len += a.distance(b) / m_triangle_sz;
  }

  return len;
}

bool FloorPlan::checkErrors(PlanErrorChecker* checker) const {
  if (!checker)
    return false;

  checker->visitCheckBasicProperties();

  // Check if the number of nodes is enough
  size_t sz_nodes = m_nodes.size();
  if (sz_nodes < 3 && checker->visitInsufficientNodes(sz_nodes))
    return false;

  // Check if the triangle size is correct
  if (m_triangle_sz <= 0.0 && checker->visitInvalidTriangleSize(m_triangle_sz))
    return false;

  // Check if the height is divisible by the triangle size and if it is greater than 0
  double tmp = m_height / m_triangle_sz;
  if ((!utils::isInteger(tmp) || tmp <= 0.0) && checker->visitInvalidHeight(m_height))
    return false;

  checker->visitCheckSegmentsProperties();

  double total = 0.0;
  for (size_t i = 1; i <= sz_nodes; ++i) {
    Point2 a = m_nodes[i - 1];
    Point2 b = m_nodes[i % sz_nodes];

    Line2 ab(a, b);
    double slope = ab.slope();

    // Check if the segment length is divisible by the triangle size
    if (!utils::isInteger(ab.length() / m_triangle_sz) &&
        checker->visitInvalidSegmentLength(ab))
      return false;

    // Check if the slope is divisible by the triangle size or if it is vertical
    if (slope != std::numeric_limits<double>::max() &&
        !utils::isInteger(slope / m_triangle_sz) &&
        checker->visitInvalidSegmentSlope(ab))
      return false;

    // This sum is used to know if points are in CCW order
    total += (b.getX() - a.getX()) * (a.getY() + b.getY());
  }

  checker->visitCheckPointsOrder();

  // Points are not expressed in CCW order
  if (total >= 0.0 && checker->visitNotCCWOrder())
    return false;

  checker->visitCheckRepeatedPoints();
  checker->visitCheckSegmentsIntersections();

  // Check if there are no repeated points or intersecting segments
  for (size_t i = 0; i < sz_nodes - 1; ++i) {
    Line2 a(m_nodes[i], m_nodes[i + 1]);

    for (size_t j = i + 1; j < sz_nodes; ++j)
      if (m_nodes[i] == m_nodes[j] && checker->visitRepeatedPoint(m_nodes[i]))
        return false;

    // Don't check consecutive segments, because thay always intersect
    // (they have a point in common)
    for (size_t j = i + 2; j < sz_nodes; ++j) {
      if (i != 0 || j < sz_nodes - 1) {
        Line2 b(m_nodes[j], m_nodes[(j + 1) % sz_nodes]);
        if (a.intersects(b) && checker->visitIntersectingSegments(a, b))
          return false;
      }
    }
  }

  return true;
}

bool FloorPlan::valid() const {
  AbortPlanErrorChecker checker;
  return checkErrors(&checker);
}

bool FloorPlan::pointInside(const Point2& p) const {
  int wn = 0;
  size_t sz_nodes = m_nodes.size();

  for (size_t i = 0; i < sz_nodes; ++i) {
    size_t next = (i+1) % sz_nodes;
    Line2 line(m_nodes[i], m_nodes[next]);

    if (m_nodes[i].getY() <= p.getY()) {
      if (m_nodes[next].getY() > p.getY()) {
        if (p.isLeft(line))
          ++wn;
      }
    }
    else {
      if (m_nodes[next].getY() <= p.getY()) {
        if (p.isRight(line))
          --wn;
      }
    }
  }

  return wn != 0;
}

bool FloorPlan::pointInBoundary(const Point2& p) const {
  size_t sz_nodes = m_nodes.size();
  for (size_t i = 0; i < sz_nodes; ++i) {
    Point2 a = m_nodes[i];
    Point2 b = m_nodes[(i + 1) % sz_nodes];

    if (Line2(a, b).contains(p))
      return true;
  }

  return false;
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
