#pragma once

// Main Project Header
// Specify includes needed by most file
// as well as constants, using declaration
// forward class declarations and a few
// utility functions


// Global headers
#include <limits>
#include <iostream>
#include <cmath>
// #include <algorithm>
#include <memory>

/*
You must #define NDEBUG (or use the flag -DNDEBUG with g++) this will disable
assert as long as it's defined before the inclusion of the assert header file.
*/
// Removing temporarily our assert() calls
#ifndef NDEBUG
#define NDEBUG
#endif
#include <assert.h>


// Type definitions
typedef float Float;


// Global constants
constexpr Float MaxFloat = std::numeric_limits<Float>::max();
constexpr Float Infinity = std::numeric_limits<Float>::infinity();
constexpr Float Pi = 3.14159265358979323846;
constexpr Float InvPi = 0.31830988618379067154;
constexpr Float Inv2Pi  = 0.15915494309189533577;
#define MachineEpsilon (std::numeric_limits<Float>::epsilon() * 0.5)
constexpr Float kEpsilon = 1e-8;

constexpr Float ImageClampMax = 100;


// Using
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;


// Forward Class Declaration
// class Ray;
struct Intersection;
class Scene;
// class Material;
template <typename T>
struct Vector3 ;
typedef Vector3<Float> Vec3;
typedef Vector3<Float> Color;
typedef Vector3<Float> Normal;
typedef Vector3<Float> Point;

struct RenderSettings {
  int image_width{200};
  int image_height{100};
  Float image_aspect_ratio{2};
  int tile_size{16};
  int pixel_samples{20};
  int max_ray_depth{5};
  bool normalOnly{false};
  int max_threads{-1};
  char const *image_out{"./out.png"};
};


// Utility Functions

template <typename T>
bool IsNan(const T &v) {
    return std::isnan(v);
}

template <typename T, typename U>
T Min(const T &t, const U &u) {
  return (t<=u) ? t : u;
}

template <typename T, typename U>
T Max(const T &t, const U &u) {
  return (t>=u) ? t : u;
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