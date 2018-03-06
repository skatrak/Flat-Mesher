#ifndef FLATMESHER_UTILS_H_
#define FLATMESHER_UTILS_H_

#include <cmath>

namespace flat { namespace utils {

const double PI = 3.14159265358979323846;
const double DOUBLE_EPSILON = 0.00001;

inline bool isInteger(double n, double epsilon = DOUBLE_EPSILON) {
  return fabs(round(n) - n) <= epsilon;
}

inline bool areEqual(double x, double y, double epsilon = DOUBLE_EPSILON) {
  return std::abs(x - y) < epsilon;
}

inline bool greater(double x, double y, double epsilon = DOUBLE_EPSILON) {
  return x > y && !areEqual(x, y, epsilon);
}

inline bool less(double x, double y, double epsilon = DOUBLE_EPSILON) {
  return x < y && !areEqual(x, y, epsilon);
}

inline bool greaterEqual(double x, double y, double epsilon = DOUBLE_EPSILON) {
  return x >= y || areEqual(x, y, epsilon);
}

inline bool lessEqual(double x, double y, double epsilon = DOUBLE_EPSILON) {
  return x <= y || areEqual(x, y, epsilon);
}

}} // namespace flat::utils

#endif // FLATMESHER_UTILS_H_
