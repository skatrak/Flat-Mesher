#include "FlatMesher/Point3.h"

using namespace flat;

Point3& Point3::operator=(const Point3& p) {
  setX(p.getX());
  setY(p.getY());
  setZ(p.getZ());

  return *this;
}

Point3 Point3::operator+(const Point3& p) const {
  return Point3(m_x + p.m_x, m_y + p.m_y, m_z + p.m_z);
}

Point3 Point3::operator-(const Point3& p) const {
  return Point3(m_x - p.m_x, m_y - p.m_y, m_z - p.m_z);
}

Point3 Point3::operator*(double n) const {
  return Point3(m_x * n, m_y * n, m_z * n);
}

Point3 Point3::operator/(double n) const {
  return Point3(m_x / n, m_y / n, m_z / n);
}

bool Point3::operator==(const Point3& p) const {
  return m_x == p.m_x && m_y == p.m_y && m_z == p.m_z;
}

std::ostream& operator<<(std::ostream& os, const Point3& p) {
  os << p.getX() << " " << p.getY() << " " << p.getZ();
  return os;
}

std::istream& operator>>(std::istream& is, Point3& p) {
  double x, y, z;
  is >> x >> y >> z;

  p.setX(x);
  p.setY(y);
  p.setY(z);

  return is;
}
