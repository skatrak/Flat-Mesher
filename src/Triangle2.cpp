#include "FlatMesher/Triangle2.h"
#include "FlatMesher/Utils.h"

using namespace flat;

double Triangle2::area() const {
  return (m_a.getX() * (m_b.getY() - m_c.getY()) +
          m_b.getX() * (m_c.getY() - m_a.getY()) +
          m_c.getX() * (m_a.getY() - m_b.getY())) / 2;
}

double Triangle2::cross() const {
  return m_a.getX() * (m_b.getY() - m_c.getY()) +
         m_a.getY() * (m_c.getX() - m_b.getX()) +
         m_b.getX() * m_c.getY() - m_b.getY() * m_c.getX();
}

bool Triangle2::colinear() const {
  return utils::areEqual((m_a.getY() - m_b.getY()) * (m_a.getX() - m_c.getX()),
                          (m_a.getY() - m_c.getY()) * (m_a.getX() - m_b.getX()));
}
