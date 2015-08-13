#include "FlatMesher/Line2.h"
#include "FlatMesher/Triangle2.h"
#include "FlatMesher/Utils.h"

#include <cmath>
#include <limits>

using namespace flat;

Rectangle Line2::boundingBox() const {
  double leftmost = std::fmin(m_a.getX(), m_b.getX());
  double rightmost = std::fmax(m_a.getX(), m_b.getX());
  double topmost = std::fmax(m_a.getY(), m_b.getY());
  double lowermost = std::fmin(m_a.getY(), m_b.getY());

  return Rectangle(topmost, lowermost, leftmost, rightmost);
}

double Line2::slope() const {
  if (utils::areEqual(m_a.getX(), m_b.getX()))
    return std::numeric_limits<double>::max();

  return (m_b.getY() - m_a.getY()) / (m_b.getX() - m_a.getX());
}

double Line2::length() const {
  return m_a.distance(m_b);
}

bool Line2::valid() const {
  return m_a != m_b;
}

bool Line2::contains(const Point2& p) const {
  // Vertical line
  if (utils::areEqual(m_a.getX(), m_b.getX())) {
    double min_y = std::fmin(m_a.getY(), m_b.getY());
    double max_y = std::fmax(m_a.getY(), m_b.getY());

    return utils::areEqual(p.getX(), m_a.getX()) && p.getY() >= min_y && p.getY() <= max_y;
  }

  // General case (y = mx + b)
  double m = slope();
  double b = m_a.getY() - m * m_a.getX();

  return utils::areEqual(p.getY(), m * p.getX() + b) && boundingBox().contains(p);
}

bool Line2::intersects(const Line2& l) const {
  // If they are parallel and their bounding boxes overlap
  if (utils::areEqual(std::abs(slope()), std::abs(l.slope())) && boundingBox().intersects(l.boundingBox()))
    return true;

  // If one of the edges of any of the lines is part of the other line
  if (this->contains(l.m_a) || this->contains(l.m_b) || l.contains(m_a) || l.contains(m_b))
    return true;

  // General case
  // http://jeffe.cs.illinois.edu/teaching/373/notes/x06-sweepline.pdf F.2
  return (Triangle2(m_a, l.m_a, l.m_b).ccw() != Triangle2(m_b, l.m_a, l.m_b).ccw()) &&
         (Triangle2(m_a, m_b, l.m_a).ccw()   != Triangle2(m_a, m_b, l.m_b).ccw());
}

bool Line2::operator==(const Line2& l) const {
  return m_a == l.m_a && m_b == l.m_b;
}
