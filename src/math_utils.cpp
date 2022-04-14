#include "math_utils.h"

#include <cmath>

Point2D const Point2D::operator*(const float scalar) const {
  return { this->x * scalar, this->y * scalar };
}

float deg_to_rad(const float deg) {
  return deg * (float)M_PI / 180.f;
}

float dist(const Point2D p0, const Point2D p1) {
  return sqrt(pow(p1.x - p0.x, 2) + pow(p1.y - p0.y, 2));
}

Point2D interpolate(float t, const Point2D p0, const Point2D p1, const Point2D p2, const Point2D p3) {
  return p0 * (pow(-t, 3) + 3 * pow(t, 2) - 3 * t + 1);
}
