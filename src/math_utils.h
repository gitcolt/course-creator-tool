#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <algorithm>

struct Point {
  Point operator*(const float scalar);
  float x;
  float y;
};

#include "imgui.h"

float deg_to_rad(float deg);

Point interpolate(float t, Point p0, Point p1, Point p2, Point p3);

template <typename T>
T clamp(const T& x, const T& min_val, const T& max_val) {
  return std::min(max_val, std::max(x, min_val));
}

#endif
