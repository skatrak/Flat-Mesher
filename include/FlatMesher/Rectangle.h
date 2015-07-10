#ifndef FLATMESHER_RECTANGLE_H_
#define FLATMESHER_RECTANGLE_H_

#include "Point2.h"

namespace flat {

class Rectangle {
public:
  Rectangle() = default;
  Rectangle(const Rectangle& rect) = default;
  Rectangle(const Point2& tl, const Point2& lr);
  Rectangle(const Point2& tl, double width, double height);
  Rectangle(double top, double bottom, double left, double right);

  bool contains(const Point2& p) const;
  bool intersects(const Rectangle& r) const;
  bool valid() const;

  Point2 getTopLeft() const { return m_tl; }
  Point2 getTopRight() const { return Point2(m_lr.getX(), m_tl.getY()); }
  Point2 getLowerLeft() const { return Point2(m_tl.getX(), m_lr.getY()); }
  Point2 getLowerRight() const { return m_lr; }
  double getWidth() const { return m_lr.getX() - m_tl.getX(); }
  double getHeight() const { return m_tl.getY() - m_lr.getY(); }

  void setTopLeft(const Point2& p);
  void setWidth(double width);
  void setHeight(double height);

  Rectangle& operator=(const Rectangle& rect) = default;

private:
  // top-left and lower-right points
  Point2 m_tl, m_lr;

};

} // namespace flat

#endif // FLATMESHER_RECTANGLE_H_
