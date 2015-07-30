#ifndef FLATMESHER_UTILS_H_
#define FLATMESHER_UTILS_H_

#include <cmath>

namespace flat { namespace utils {

const double PI = 3.14159265358979323846;
const double DOUBLE_EPSILON = 0.00001;

inline bool isInteger(double n) {
  return n == (int) n;
}

inline bool areEqual(double x, double y, double epsilon = DOUBLE_EPSILON) {
  return std::abs(x - y) < epsilon;
}

}} // namespace flat::utils

#endif // FLATMESHER_UTILS_H_
