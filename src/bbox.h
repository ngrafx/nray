#pragma once

#include "nray.h"
#include "geometry.h"

// BBox class
// This is mainly used by the BVH Primitive to speed up intersection tests
class BBox {
  public:
        BBox() {}
        BBox(const Vec3& a, const Vec3& b) { _min = a; _max = b; }

        Vec3 Min() const {return _min; }
        Vec3 Max() const {return _max; }

        bool Intersect(const Ray& r, Float tmin, Float tmax) const ;

        Float Area() const;

        int LongestAxis() const;

  private:
        Vec3 _min;
        Vec3 _max;
};

// BBox utility Functions
BBox BBoxUnion(BBox &box0, BBox &box1);

