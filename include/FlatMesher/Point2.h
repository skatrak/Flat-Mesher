#ifndef FLATMESHER_POINT2_H_
#define FLATMESHER_POINT2_H_

#include <iostream>

namespace flat {

class Point2 {
public:
  explicit Point2(double x = 0.0, double y = 0.0): m_x(x), m_y(y) {}
  Point2(const Point2& p): m_x(p.m_x), m_y(p.m_y) {}

  double getX() const { return m_x; }
  double getY() const { return m_y; }

  void setX(double x) { m_x = x; }
  void setY(double y) { m_y = y; }

  double distance(const Point2& p) const;

  Point2& operator=(const Point2& p);
  Point2 operator+(const Point2& p) const;
  Point2 operator-(const Point2& p) const;
  Point2 operator*(double n) const;
  Point2 operator/(double n) const;

  bool operator==(const Point2& p) const;
  bool operator!=(const Point2& p) const { return !(*this == p); }

  static double cross(const Point2& p1, const Point2& p2);
  static double dot(const Point2& p1, const Point2& p2);

private:
  double m_x, m_y;

};

} // namespace flat

std::ostream& operator<<(std::ostream& os, const flat::Point2& p);
std::istream& operator>>(std::istream& is, flat::Point2& p);

#endif // FLATMESHER_POINT2_H_
