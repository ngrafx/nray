/*
    nray source code is Copyright(c) 2020
                        Nicolas Delbecq

    This file is part of nray.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */
#pragma once

// #ifndef NRAY_H
// #define NRAY_H


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
const Float MaxFloat = std::numeric_limits<Float>::max();
const Float Infinity = std::numeric_limits<Float>::infinity();
const Float Pi = 3.14159265358979323846;
const Float InvPi = 0.31830988618379067154;


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

template <typename T, typename U, typename V>
inline T Clamp(const T &t, const U &min_, const V &max_) {
  if (t < min_) return min_;
  if (t > max_) return max_;
  return t;
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