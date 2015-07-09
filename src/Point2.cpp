#include "FlatMesher/Point2.h"

#include <cmath>

using namespace flat;

double Point2::distance(const Point2& p) const {
  double x_diff = p.getX() - getX();
  double y_diff = p.getY() - getY();

  return sqrt(x_diff * x_diff + y_diff * y_diff);
}

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

double Point2::cross(const Point2& p1, const Point2& p2) {
  return p1.getX() * p2.getY() - p1.getY() * p2.getX();
}

double Point2::dot(const Point2& p1, const Point2& p2) {
  return p1.getX() * p2.getX() + p1.getY() * p2.getY();
}
