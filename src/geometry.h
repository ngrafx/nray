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

// Vector3 is the base class 

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
        return Vector3(x * s, y * s, z * s);
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
        return Vector3(x * k, y * k, z * k);
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

template <typename T, typename U>
inline Vector3<T> operator*(U s, const Vector3<T> &v) {
    return v * s;
}

// Vector3 typedefs
// TODO: They need to be child structs so we can override how they transform

typedef Vector3<Float> Vec3;
typedef Vector3<Float> Color;
typedef Vector3<Float> Normal;
typedef Vector3<Float> Point;


// Vector3 utility functions

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

template <typename T>
Vector3<T> Reflect(const Vector3<T>& v, const Vector3<T>& n) {
    return v - n * 2*Dot(v,n);
}

template <typename T>
Vector3<T> Refract(const Vector3<T>& uv, const Vector3<T>& n, Float etai_over_etat) {
    auto cos_theta = Min(Dot(-uv, n), 1.0);
    Vector3<T> r_out_parallel =  etai_over_etat * (uv + cos_theta*n);
    Vector3<T> r_out_perp = -sqrt(1.0 - r_out_parallel.LengthSquared()) * n;
    return r_out_parallel + r_out_perp;
}


// Ray class

class Ray 
{
public:
    Ray() : _tMax(Infinity), _time(0.f) {}
    Ray(const Point &o, const Vec3 &d) : _origin(o), _direction(d) {}

    Point operator() (Float t) const { return _origin + _direction * t; }

    Point Origin() const { return _origin;}
    Vec3 Direction() const { return _direction;}
    Float Time() const { return _time;}
    Float TMax() const { return _tMax;}

private:
    Point _origin; // Ray origin
    Vec3 _direction; // Ray Direction
    Float _time; // Ray time
    Float _tMax; // Ray max distance
};

// Ray utility Functions

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "[ origin=" << r.Origin() << ", direction=" << r.Direction() << ", time="<< r.Time() << ", tMax=" << r.TMax() << " ]";
    return os;
}