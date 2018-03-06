#include "FlatMesher/Rectangle.h"

#include "FlatMesher/Utils.h"

using namespace flat;

Rectangle::Rectangle(const Point2& tl, const Point2& lr):
 m_tl(tl), m_lr(lr) {}

Rectangle::Rectangle(double top, double bottom, double left, double right):
  m_tl(left, top), m_lr(right, bottom) {}

Rectangle::Rectangle(const Point2& tl, double width, double height):
  m_tl(tl), m_lr(tl.getX() + width, tl.getY() - height) {}

bool Rectangle::contains(const Point2& p) const {
  using namespace utils;

  return lessEqual(m_tl.getX(), p.getX()) &&
         greaterEqual(m_tl.getY(), p.getY()) &&
         greaterEqual(m_lr.getX(), p.getX()) &&
         lessEqual(m_lr.getY(), p.getY());
}

bool Rectangle::intersects(const Rectangle& r) const {
  using namespace utils;

  return lessEqual(m_tl.getX(), r.m_lr.getX()) &&
         lessEqual(r.m_tl.getX(), m_lr.getX()) &&
         greater(m_tl.getY(), r.m_lr.getY()) &&
         greater(r.m_tl.getY(), m_lr.getY());
}

bool Rectangle::valid() const {
  return utils::less(m_tl.getX(), m_lr.getX()) &&
         utils::greater(m_tl.getY(), m_lr.getY());
}

void Rectangle::setTopLeft(const Point2& p) {
  Point2 diff = p - m_tl;
  
  m_tl = p;
  m_lr = m_lr + diff;
}

void Rectangle::setWidth(double width) {
  m_lr = Point2(m_tl.getX() + width, m_lr.getY());
}

void Rectangle::setHeight(double height) {
  m_lr = Point2(m_lr.getX(), m_tl.getY() - height);
}

bool flat::Rectangle::operator==(const Rectangle& p) const {
  return m_tl == p.m_tl && m_lr == p.m_lr;
}
