#ifndef FLATMESHER_INDEXTRIANGLE_H_
#define FLATMESHER_INDEXTRIANGLE_H_

#include <iostream>

namespace flat {

class IndexTriangle {
public:
  IndexTriangle(size_t i, size_t j, size_t k): m_i(i), m_j(j), m_k(k) {}
  IndexTriangle(const IndexTriangle& t) = default;

  size_t getI() const { return m_i; }
  size_t getJ() const { return m_j; }
  size_t getK() const { return m_k; }

  void setI(size_t i) { m_i = i; }
  void setJ(size_t j) { m_j = j; }
  void setK(size_t k) { m_k = k; }

  IndexTriangle offset(size_t index_offset) const;
  void invertRotation();

  IndexTriangle& operator=(const IndexTriangle&) = default;

private:
  size_t m_i, m_j, m_k;

};

} // namespace flat

std::ostream& operator<<(std::ostream& os, const flat::IndexTriangle& triangle);
std::istream& operator>>(std::istream& is, flat::IndexTriangle& triangle);

#endif // FLATMESHER_INDEXTRIANGLE_H_
