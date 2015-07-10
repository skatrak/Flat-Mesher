#include "FlatMesher/IndexTriangle.h"

#include <algorithm>

using namespace flat;

IndexTriangle IndexTriangle::offset(size_t index_offset) const {
  return IndexTriangle(m_i + index_offset, m_j + index_offset, m_k + index_offset);
}

void IndexTriangle::invertRotation() {
  std::swap(m_j, m_k);
}
