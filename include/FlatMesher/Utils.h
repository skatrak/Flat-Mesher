#ifndef FLATMESHER_UTILS_H_
#define FLATMESHER_UTILS_H_

namespace flat { namespace utils {

inline bool isInteger(double n) {
  return n == (int) n;
}

}} // namespace flat::utils

#endif // FLATMESHER_UTILS_H_
