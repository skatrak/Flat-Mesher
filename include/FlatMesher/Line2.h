#ifndef FLATMESHER_LINE2_H_
#define FLATMESHER_LINE2_H_

#include "Point2.h"
#include "Rectangle.h"

namespace flat {

class Line2 {
public:
  Line2() = default;
  Line2(Point2 a, Point2 b): m_a(a), m_b(b) {}
  Line2(const Line2& l) = default;

  Point2 getA() const { return m_a; }
  Point2 getB() const { return m_b; }

  void setA(const Point2& a) { m_a = a; }
  void setB(const Point2& b) { m_b = b; }

  Rectangle boundingBox() const;
  double slope() const;

  bool valid() const;
  bool contains(const Point2& p) const;
  bool intersects(const Line2& l) const;

  bool operator==(const Line2& l) const;
  bool operator!=(const Line2& l) const { return !(*this == l); }

  Line2& operator=(const Line2& l) = default;

private:
  Point2 m_a, m_b;

};

} // namespace flat

#endif // FLATMESHER_LINE2_H_
