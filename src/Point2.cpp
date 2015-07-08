#include "FlatMesher/Point2.h"

using namespace flat;

Point2& Point2::operator=(const Point2& p) {
  setX(p.getX());
  setY(p.getY());

  return *this;
}

Point2 Point2::operator+(const Point2& p) const {
  return Point2(m_x + p.m_x, m_y + p.m_y);
}

Point2 Point2::operator-(const Point2& p) const {
  return Point2(m_x - p.m_x, m_y - p.m_y);
}

Point2 Point2::operator*(double n) const {
  return Point2(m_x * n, m_y * n);
}

Point2 Point2::operator/(double n) const {
  return Point2(m_x / n, m_y / n);
}

bool Point2::operator==(const Point2& p) const {
  return m_x == p.m_x && m_y == p.m_y;
}

std::ostream& operator<<(std::ostream& os, const Point2& p) {
  os << p.getX() << " " << p.getY();
  return os;
}

std::istream& operator>>(std::istream& is, Point2& p) {
  double x, y;
  is >> x >> y;

  p.setX(x);
  p.setY(y);

  return is;
}
