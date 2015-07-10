#ifndef FLATMESHER_INDEXTRIANGLE_H_
#define FLATMESHER_INDEXTRIANGLE_H_

namespace flat {

class IndexTriangle {
public:
  IndexTriangle(size_t i, size_t j, size_t k): m_i(i), m_j(j), m_k(k) {}
  IndexTriangle(const IndexTriangle& t) = default;

  IndexTriangle offset(size_t index_offset) const;
  void invertRotation();

  size_t getI() const { return m_i; }
  size_t getJ() const { return m_j; }
  size_t getK() const { return m_k; }

  IndexTriangle& operator=(const IndexTriangle&) = default;

private:
  size_t m_i, m_j, m_k;

};

} // namespace flat

#endif // FLATMESHER_INDEXTRIANGLE_H_
