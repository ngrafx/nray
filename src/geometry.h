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

#include "nray.h"


template <typename T>
struct Vector3 
{
    // Public Members
    T x,y,z;

    Vector3 () {x = y = z = 0;}
    Vector3 (T x_, T y_, T z_) : x(x_), y(y_), z(z_)  {}

    Vector3<T>& operator+() const { return *this; }
    
    Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
    
    T operator[](unsigned i) const { 
        assert(i>=0 && i<=2);
        if (i == 0) return x;
        else if (i == 1) return y;
        return z;
    }

    T& operator[](unsigned i) {
        assert(i>=0 && i<=2);
        if (i == 0) return x;
        else if (i == 1) return y;
        return z;
    }

    Vector3<T> operator+(const Vector3<T> &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3<T>& operator+=(const Vector3<T> &v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    Vector3<T> operator-(const Vector3<T> &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3<T>& operator-=(const Vector3<T> &v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }

    Vector3<T> operator*(const Vector3<T> &v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    Vector3<T>& operator*=(const Vector3<T> &v) {
        x *= v.x; y *= v.y; z *= v.z;
        return *this;
    }

    template <typename U>
    Vector3<T> operator*(U s) const {
        return Vector3<T>(x * s, y * s, z * s);
    }

    template <typename U>
    Vector3<T>& operator*=(U s) {
        assert(!IsNan(s));
        x *= s; y *= s; z *= s;
        return *this;
    }

    Vector3<T> operator/(const Vector3<T> &v) const {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }

    Vector3<T>& operator/=(const Vector3<T> &v) {
        x /= v.x; y /= v.y; z /= v.z;
        return *this;
    }

    template <typename U>
    Vector3<T> operator/(U s) const {
        Float k = (Float) 1 / s;
        return Vector3<T>(x * k, y * k, z * k);
    }

    template <typename U>
    Vector3<T>& operator/=(U s) {
        Float k = (Float) 1 / s;
        x *= k; y *= k; z *= k;
        return *this;
    }

    bool operator==(const Vector3<T> &v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const Vector3<T> &v) const {
        return x != v.x || y != v.y || z != v.z;
    }

    T Length() const {
        return std::sqrt(LengthSquared());
    }
    T LengthSquared() const {
        return x * x + y * y + z * z;
    }

    bool HasNan() const {
        return IsNan(x) || IsNan(y) || IsNan(z);
    }
};

typedef Vector3<Float> Vec3;
typedef Vector3<Float> Color;
typedef Vector3<Float> Normal;
typedef Vector3<Float> Point;

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
    os << "[ " << v.x << ", " << v.y << ", "<< v.z << " ]";
    return os;
}

template<typename T>
inline std::istream& operator>>(std::istream &is, Vector3<T> &v) {
    is >> v.x >> v.y >> v.z;
    return is;
}

template <typename T>
inline Vector3<T> Normalize(const Vector3<T> &v) {
    return v / v.Length();
}

template<typename T>
inline T Dot(const Vector3<T> &u, const Vector3<T> &v) {
    assert(!u.HasNan() && !v.HasNan());
    return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
inline Vector3<T> Cross(const Vector3<T> &u, const Vector3<T> &v) {
    assert(!u.HasNan() && !v.HasNan());
    return Vector3<T>(  (u.y*v.z - u.z*v.y),
                       -(u.x*v.z - u.z*v.x),
                        (u.x*v.y - u.y*v.x));
}

template <typename T>
inline Float Distance(const Vector3<T> &u, const Vector3<T> &v) {
    assert(!u.HasNan() && !v.HasNan());
    return (u - v).Length();
}

template <typename T>
inline Float DistanceSquared(const Vector3<T> &u, const Vector3<T> &v) {
    assert(!u.HasNan() && !v.HasNan());
    return (u - v).LengthSquared();
}