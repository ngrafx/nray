
#include "bbox.h"

bool BBox::Intersect(const Ray& r, Float tmin, Float tmax) const {
  for (int a = 0; a < 3; a++) {
      auto invD = 1.0f / r.Direction()[a];
      auto t0 = (_min[a] - r.Origin()[a]) * invD;
      auto t1 = (_max[a] - r.Origin()[a]) * invD;
      if (invD < 0.0f)
          std::swap(t0, t1);
      tmin = t0 > tmin ? t0 : tmin;
      tmax = t1 < tmax ? t1 : tmax;
      if (tmax <= tmin)
          return false;
  }
  return true;
}


Float BBox::Area() const {
    auto a = _max.x - _min.x;
    auto b = _max.y - _min.y;
    auto c = _max.z - _min.z;
    return 2*(a*b + b*c + c*a);
}

int BBox::LongestAxis() const {
    auto a = _max.x - _min.x;
    auto b = _max.y - _min.y;
    auto c = _max.z - _min.z;
    if (a > b && a > c)
        return 0;
    else if (b > c)
        return 1;
    else
        return 2;
}

BBox BBoxUnion(BBox &box0, BBox &box1) {
    Vec3 small(  Min(box0.Min().x, box1.Min().x),
                 Min(box0.Min().y, box1.Min().y),
                 Min(box0.Min().z, box1.Min().z));

    Vec3 big(  Max(box0.Max().x, box1.Max().x),
               Max(box0.Max().y, box1.Max().y),
               Max(box0.Max().z, box1.Max().z));

    return BBox(small,big);
}