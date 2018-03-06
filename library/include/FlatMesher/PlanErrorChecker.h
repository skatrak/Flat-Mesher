#ifndef FLATMESHER_PLANERRORCHECKER_H_
#define FLATMESHER_PLANERRORCHECKER_H_

namespace flat {

class Line2;
class Point2;

class PlanErrorChecker {
public:
  virtual ~PlanErrorChecker() = default;

  // This methods are called to know which part of the processing is being done
  virtual void visitCheckBasicProperties() {}
  virtual void visitCheckSegmentsProperties() {}
  virtual void visitCheckPointsOrder() {}
  virtual void visitCheckRepeatedPoints() {}
  virtual void visitCheckSegmentsIntersections() {}

  // This methods are called every time an error is found. If they return 'true',
  // the check is immediately aborted
  virtual bool visitInsufficientNodes(size_t nodes) = 0;
  virtual bool visitInvalidTriangleSize(double triangle_size) = 0;
  virtual bool visitInvalidHeight(double height) = 0;
  virtual bool visitInvalidSegmentLength(const Line2& segment) = 0;
  virtual bool visitInvalidSegmentSlope(const Line2& segment) = 0;
  virtual bool visitNotCCWOrder() = 0;
  virtual bool visitRepeatedPoint(const Point2& point) = 0;
  virtual bool visitIntersectingSegments(const Line2& first,
                                         const Line2& second) = 0;

};

} // namespace flat

#endif // FLATMESHER_PLANERRORCHECKER_H_
