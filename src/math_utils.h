#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <algorithm>

float deg_to_rad(float deg);

template <typename T>
T clamp(const T& x, const T& min_val, const T& max_val) {
  return std::min(max_val, std::max(x, min_val));
}

#endif
