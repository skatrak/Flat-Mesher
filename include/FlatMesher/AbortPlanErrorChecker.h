#ifndef FLATMESHER_ABORTPLANERRORCHECKER_H_
#define FLATMESHER_ABORTPLANERRORCHECKER_H_

#include "PlanErrorChecker.h"

namespace flat {

class AbortPlanErrorChecker: public PlanErrorChecker {
public:
  virtual ~AbortPlanErrorChecker() = default;

  virtual bool visitInsufficientNodes(size_t /*nodes*/) { return true; }
  virtual bool visitInvalidTriangleSize(double /*triangle_size*/) { return true; }
  virtual bool visitInvalidHeight(double /*height*/) { return true; }
  virtual bool visitInvalidSegmentLength(const Line2& /*segment*/) { return true; }
  virtual bool visitInvalidSegmentSlope(const Line2& /*segment*/) { return true; }
  virtual bool visitNotCCWOrder() { return true; }
  virtual bool visitRepeatedPoint(const Point2& /*point*/) { return true; }
  virtual bool visitIntersectingSegments(const Line2& /*first*/,
                                         const Line2& /*second*/) { return true; }

};

} // namespace flat

#endif // FLATMESHER_ABORTPLANERRORCHECKER_H_
