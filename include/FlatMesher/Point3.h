#ifndef FLATMESHER_POINT3_H_
#define FLATMESHER_POINT3_H_

#include <iostream>

#include "Point2.h"

namespace flat {

class Point3 {
public:
  explicit Point3(double x = 0.0, double y = 0.0, double z = 0.0): m_x(x), m_y(y), m_z(z) {}
  Point3(const Point3& p): m_x(p.m_x), m_y(p.m_y) {}
  Point3(const Point2& p, double z = 0.0): m_x(p.getX()), m_y(p.getY()), m_z(z) {}

  double getX() const { return m_x; }
  double getY() const { return m_y; }
  double getZ() const { return m_y; }

  void setX(double x) { m_x = x; }
  void setY(double y) { m_y = y; }
  void setZ(double z) { m_z = z; }

  Point3& operator=(const Point3& p);
  Point3 operator+(const Point3& p) const;
  Point3 operator-(const Point3& p) const;
  Point3 operator*(double n) const;
  Point3 operator/(double n) const;

  bool operator==(const Point3& p) const;
  bool operator!=(const Point3& p) const { return !(*this == p); }

private:
  double m_x, m_y, m_z;

};

} // namespace flat

std::ostream& operator<<(std::ostream& os, const flat::Point3& p);
std::istream& operator>>(std::istream& is, flat::Point3& p);

#endif // FLATMESHER_POINT3_H_
