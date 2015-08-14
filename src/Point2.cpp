#include "FlatMesher/Point2.h"

#include "FlatMesher/Line2.h"
#include "FlatMesher/Utils.h"

#include <cmath>

using namespace flat;

double Point2::distance(const Point2& p) const {
  double x_diff = p.getX() - getX();
  double y_diff = p.getY() - getY();

  return sqrt(x_diff * x_diff + y_diff * y_diff);
}

bool flat::Point2::isLeft(const Line2 & l) const {
  return utils::greater(relativeToLine(l), 0.0);
}

bool flat::Point2::isRight(const Line2 & l) const {
  return utils::less(relativeToLine(l), 0.0);
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
  return utils::areEqual(m_x, p.m_x) && utils::areEqual(m_y, p.m_y);
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

double flat::Point2::relativeToLine(const Line2 & l) const {
  return ((l.getB().getX() - l.getA().getX()) * (getY() - l.getA().getY()) -
          (getX() - l.getA().getX()) * (l.getB().getY() - l.getA().getY()));
}
