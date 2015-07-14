#include "FlatMesher/FloorPlan.h"
#include "FlatMesher/Line2.h"
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

  // Check if there are no repeated points or intersecting segments
  for (size_t i = 0; i < sz_nodes - 1; ++i) {
    Line2 a(m_nodes[i], m_nodes[i + 1]);

    for (size_t j = i + 1; j < sz_nodes; ++j)
      if (m_nodes[i] == m_nodes[j])
        return false;

    // Don't check consecutive segments, because thay always intersect
    // (they have a point in common)
    for (size_t j = i + 2; j < sz_nodes; ++j) {
      if (i != 0 || j < sz_nodes - 1) {
        Line2 b(m_nodes[j], m_nodes[(j + 1) % sz_nodes]);
        if (a.intersects(b))
          return false;
      }
    }
  }

  return true;
}

bool FloorPlan::pointInside(const Point2& p) const {
  static const double Q_WEIGHTS [] = {
    0.173927422568727,
    0.326072577431273,
    0.326072577431273,
    0.173927422568727
  };

  static const double Q_POINTS [] = {
    0.930568155797026,
    0.669990521792428,
    0.330009478207572,
    0.069431844202974
  };

  static const int N_POINTS = sizeof Q_POINTS / sizeof Q_POINTS[0];

  size_t n = m_nodes.size();

  double integral = 0.0;
  for (size_t i = 1; i <= n; ++i) {
    Point2 y_start = m_nodes[i - 1];
    Point2 y_end = m_nodes[i % n];

    Point2 segment = y_end - y_start;

    double h = y_start.distance(y_end);
    Point2 normal = Point2(segment.getY(), -segment.getX()) / h;

    double local_integral = 0.0;
    for (size_t j = 0; j < N_POINTS; ++j) {
      Point2 y = y_start + (segment * Q_POINTS[j]);
      double norm_xy = p.distance(y);

      local_integral += (-1.0 / (2 * utils::M_PI) * Point2::dot(p-y, normal) /
                         (norm_xy * norm_xy)) * Q_WEIGHTS[j];
    }

    integral += local_integral * h;
  }

  return integral > 0.5;
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
