#ifndef FLATMESHER_LINE2_H_
#define FLATMESHER_LINE2_H_

#include "Point2.h"
#include "Rectangle.h"

namespace flat {

class Line2 {
public:
  Line2() {}
  Line2(Point2 a, Point2 b): m_a(a), m_b(b) {}

  Point2 getA() const { return m_a; }
  Point2 getB() const { return m_b; }

  void setA(Point2 a) { m_a = a; }
  void setB(Point2 b) { m_b = b; }

  Rectangle boundingBox() const;
  double slope() const;

  bool operator==(const Line2& l) const;
  bool operator!=(const Line2& l) const { return !(*this == l); }

private:
  Point2 m_a, m_b;

};

} // namespace flat

#endif // FLATMESHER_LINE2_H_
