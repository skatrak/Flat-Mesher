#include "FlatMesher/Point3.h"
#include "FlatMesher/Utils.h"

#include <cmath>

using namespace flat;

double Point3::distance(const Point3& p) const {
  double x_diff = p.getX() - getX();
  double y_diff = p.getY() - getY();
  double z_diff = p.getZ() - getZ();

  return sqrt(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff);
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
  return utils::areEqual(m_x, p.m_x) && utils::areEqual(m_y, p.m_y) && utils::areEqual(m_z, p.m_z);
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
  p.setZ(z);

  return is;
}
