#include "FlatMesher/Line2.h"

#include <cmath>

using namespace flat;

Rectangle Line2::boundingBox() const {
  double leftmost = std::fmin(m_a.getX(), m_b.getX());
  double rightmost = std::fmax(m_a.getX(), m_b.getX());
  double topmost = std::fmax(m_a.getY(), m_b.getY());
  double lowermost = std::fmin(m_a.getY(), m_b.getY());

  return Rectangle(topmost, lowermost, leftmost, rightmost);
}

double Line2::slope() const {
  return (m_b.getY() - m_a.getY()) / (m_b.getX() - m_a.getX());
}

bool Line2::operator==(const Line2& l) const {
  return m_a == l.m_a && m_b == l.m_b;
}
