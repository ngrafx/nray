#pragma once

#include "nray.h"
#include "rand.h"

// Vector3 is the base templated struct 
// that all the other types will inherit
// For now, Point, Normal and Color are just
// typedefs but eventually they'll be child struct

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
        // Returns if any member variable has a nan
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
    // Returns a normalized vector
    return v / v.Length();
}

template<typename T>
inline T Dot(const Vector3<T> &u, const Vector3<T> &v) {
    // Dot product between 2 vectors
    assert(!u.HasNan() && !v.HasNan());
    return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
inline Vector3<T> Cross(const Vector3<T> &u, const Vector3<T> &v) {
    // Cross product between 2 vectors
    assert(!u.HasNan() && !v.HasNan());
    return Vector3<T>(  (u.y*v.z - u.z*v.y),
                       -(u.x*v.z - u.z*v.x),
                        (u.x*v.y - u.y*v.x));
}

template <typename T>
inline void CoordinateSystem(const Vector3<T> &v1, Vector3<T> *v2,
                             Vector3<T> *v3) {
    if (std::abs(v1.x) > std::abs(v1.y))
        *v2 = Vector3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
    else
        *v2 = Vector3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
    *v3 = Cross(v1, *v2);
}

template <typename T>
inline Float Distance(const Vector3<T> &u, const Vector3<T> &v) {
    // Returns the distance between 2 vectors
    assert(!u.HasNan() && !v.HasNan());
    return (u - v).Length();
}

template <typename T>
inline Float DistanceSquared(const Vector3<T> &u, const Vector3<T> &v) {
    // Returns the squared distance between 2 vectors
    assert(!u.HasNan() && !v.HasNan());
    return (u - v).LengthSquared();
}

template <typename T>
Vector3<T> Reflect(const Vector3<T>& v, const Vector3<T>& n) {
    // Reflects a vector along a normal
    return v - n * 2*Dot(v,n);
}

template <typename T>
Vector3<T> Refract(const Vector3<T>& uv, const Vector3<T>& n, Float etai_over_etat) {
    // Refracts a vector along a normal
    auto cos_theta = Min(Dot(-uv, n), 1.0);
    Vector3<T> r_out_parallel =  etai_over_etat * (uv + cos_theta*n);
    Vector3<T> r_out_perp = -sqrt(1.0 - r_out_parallel.LengthSquared()) * n;
    return r_out_parallel + r_out_perp;
}

template <typename T>
Vector3<T> RandomUnitVector() {
    // Generates a random unit vector
    Float a = Rng::RandRange(0, 2*Pi);
    Float z = Rng::RandRange(-1, 1);
    Float r = sqrt(1 - z*z);
    return Vector3<T>(r*cos(a), r*sin(a), z);
}

template <typename T>
Vector3<T> RandomVector() {
    // Generates a vector in which every component is a random number between min and max
    return Vector3<T>(Rng::Rand01(), Rng::Rand01(), Rng::Rand01());
}

template <typename T>
Vector3<T> RandomVector(Float min, Float max) {
    // Generates a vector in which every component is a random number between min and max
    return Vector3<T>(Rng::RandRange(min, max), Rng::RandRange(min, max), Rng::RandRange(min, max));
}


template <typename T>
Vector3<T> RandomVectorInUnitSphere() {
    // Generates a random vector in a unit sphere
    while (true) {
        Vector3<T> p = RandomVector<T>(-1,1);
        if (p.LengthSquared() >= 1) continue;
        return p;
    }
}

template <typename T>
Vector3<T> RandomInUnitDisk() {
    // Generates a random vector in a unit disk
    while (true) {
        auto p = Vector3<T>(Rng::RandRange(-1,1), Rng::RandRange(-1,1), 0);
        if (p.LengthSquared() >= 1) continue;
        return p;
    }
}

template <typename T>
int MaxDimension(const Vector3<T> &v) {
    return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}

template <typename T>
Vector3<T> Abs(const Vector3<T> &v) {
    return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

template <typename T>
Vector3<T> Permute(const Vector3<T> &p, int x, int y, int z) {
    return Vector3<T>(p[x], p[y], p[z]);
}

template <typename T>
T MaxComponent(const Vector3<T> &v) {
    return std::max(v.x, std::max(v.y, v.z));
}

inline Float SphericalTheta(const Vec3 &v) {
    return std::acos(Clamp(v.z, -1, 1));
}

inline Float SphericalPhi(const Vec3 &v) {
    Float p = std::atan2(v.y, v.x);
    return (p < 0) ? (p + 2 * Pi) : p;
}




// Type of Ray
enum class RayType
{
    Primary,
    Diffuse,
    Reflect,
    Refract
};

// Ray class
class Ray 
{
public:
    Ray() : _tMax(Infinity), _time(0.f) {}
    Ray(const Point &o, const Vec3 &d, RayType type, Float time=0) : _origin(o), _direction(d), _type(type),_time(time) {}

    Point operator() (Float t) const { return _origin + _direction * t; }

    Point Origin() const { return _origin;}
    Vec3 Direction() const { return _direction;}
    Float Time() const { return _time;}
    Float TMax() const { return _tMax;}
    RayType Type() const {return _type;}

private:
    Point _origin; // Ray origin
    Vec3 _direction; // Ray Direction
    Float _time{0}; // Ray time
    Float _tMax{Infinity}; // Ray max distance
    RayType _type; // Type of Ray
};

// Ray utility Functions
inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "[ origin=" << r.Origin() << ", direction=" << r.Direction() << ", time="<< r.Time() << ", tMax=" << r.TMax() << " ]";
    return os;
}
