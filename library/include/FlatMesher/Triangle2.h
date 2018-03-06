#ifndef FLATMESHER_TRIANGLE2_H_
#define FLATMESHER_TRIANGLE2_H_

#include "Point2.h"
#include "Utils.h"

namespace flat {

class Triangle2 {
public:
  Triangle2(const Point2& a, const Point2& b, const Point2& c): m_a(a), m_b(b), m_c(c) {}
  Triangle2(const Triangle2& t) = default;

  Point2 getA() const { return m_a; }
  Point2 getB() const { return m_b; }
  Point2 getC() const { return m_c; }

  void setA(const Point2& a) { m_a = a; }
  void setB(const Point2& b) { m_b = b; }
  void setC(const Point2& c) { m_c = c; }

  double area() const;
  double cross() const;

  bool colinear() const;
  bool valid() const { return !colinear(); }
  bool ccw() const { return valid() && utils::greater(cross(), 0.0); }
  bool cw() const { return valid() && utils::less(cross(), 0.0); }

  Triangle2& operator=(const Triangle2&) = default;

private:
  Point2 m_a, m_b, m_c;

};

} // namespace flat

#endif // FLATMESHER_TRIANGLE2_H_
