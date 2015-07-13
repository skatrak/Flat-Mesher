#include "FlatMesher/IndexTriangle.h"

#include <algorithm>

using namespace flat;

IndexTriangle IndexTriangle::offset(size_t index_offset) const {
  return IndexTriangle(m_i + index_offset, m_j + index_offset, m_k + index_offset);
}

void IndexTriangle::invertRotation() {
  std::swap(m_j, m_k);
}

std::ostream& operator<<(std::ostream& os, const flat::IndexTriangle& triangle) {
  os << triangle.getI() << ' ' << triangle.getJ() << ' ' << triangle.getK();
  return os;
}

std::istream& operator>>(std::istream& is, flat::IndexTriangle& triangle) {
  size_t i, j, k;
  is >> i >> j >> k;

  triangle.setI(i);
  triangle.setJ(j);
  triangle.setK(k);

  return is;
}
