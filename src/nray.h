#pragma once


// Global headers

#include <iostream>
#include <cmath>
// #include <algorithm>
#include <memory>

/*
You must #define NDEBUG (or use the flag -DNDEBUG with g++) this will disable
assert as long as it's defined before the inclusion of the assert header file.
*/
#define NDEBUG
#include <assert.h>


// Type definitions
typedef float Float;


// Global constants
constexpr Float MaxFloat = std::numeric_limits<Float>::max();
constexpr Float Infinity = std::numeric_limits<Float>::infinity();
constexpr Float Pi = 3.14159265358979323846;
constexpr Float InvPi = 0.31830988618379067154;
#define MachineEpsilon (std::numeric_limits<Float>::epsilon() * 0.5)
constexpr Float kEpsilon = 1e-8;


// Using
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;


// Forward Class Declaration
// class Ray;
struct Intersection;
// class Material;
template <typename T>
struct Vector3 ;
typedef Vector3<Float> Vec3;
typedef Vector3<Float> Color;
typedef Vector3<Float> Normal;
typedef Vector3<Float> Point;

struct Options {
  int image_width;
  int image_height;
  Float image_aspect_ratio;
  int tile_size;
  int pixel_samples;
  int max_ray_depth;
  char const *image_out;
};


// Utility Functions

template <typename T>
bool IsNan(const T &v) {
    return std::isnan(v);
}

template <typename T, typename U>
T Min(const T &t, const U &u) {
  return (t<u) ? t : u;
}

template <typename T, typename U>
T Max(const T &t, const U &u) {
  return (t>u) ? t : u;
}

template <typename T, typename U>
T FFmin(T a, U b) { return a <= b ? a : b; }

template <typename T, typename U>
T FFmax(T a, U b) { return a >= b ? a : b; }

template <typename T, typename U, typename V>
inline T Clamp(const T &t, const U &min_, const V &max_) {
  if (t < min_) return min_;
  if (t > max_) return max_;
  return t;
}

inline Float Gamma(int n) {
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

// Convert degrees in radians
inline Float Radians(Float degrees) {
  return degrees * Pi / 180.0;
}

// Convert radians in degree
inline Float Degrees(Float radians) {
   return (180 / Pi) * radians;
}


// #endif