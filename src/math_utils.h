#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <algorithm>

struct Point2D {
  const Point2D operator*(const float scalar) const;
  float x;
  float y;
};

float deg_to_rad(float deg);

float dist(const Point2D p0, const Point2D p1);

Point2D interpolate(float t, const Point2D p0, const Point2D p1, const Point2D p2, const Point2D p3);

template <typename T>
T clamp(const T& x, const T& min_val, const T& max_val) {
  return std::min(max_val, std::max(x, min_val));
}

#endif
