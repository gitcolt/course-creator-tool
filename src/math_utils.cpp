#include "math_utils.h"

#include <cmath>

Point Point::operator*(const float scalar) {
  return { this->x * scalar, this->y * scalar };
}

float deg_to_rad(const float deg) {
  return deg * (float)M_PI / 180.f;
}

float dist(Point p0, const Point p1) {
  return sqrt(pow(p1.x - p0.x, 2) + pow(p1.y - p0.y, 2));
}

Point interpolate(float t, Point p0, Point p1, Point p2, Point p3) {
  return p0 * (pow(-t, 3) + 3 * pow(t, 2) - 3 * t + 1);
}
