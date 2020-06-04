#include "primitive.h"


// Sphere implementation

bool Sphere::Intersect(const Ray& r, Float t_min, Float t_max, Intersection& rec) const {
    Vec3 oc = r.Origin() - center;
    Float a = r.Direction().LengthSquared();
    Float half_b = Dot(oc, r.Direction());
    Float c = oc.LengthSquared() - radius*radius;
    Float discriminant = half_b*half_b - a*c;

    bool hit = false;
    Float temp;
    if (discriminant > 0) {
        Float root = std::sqrt(discriminant);

        temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) {
            hit = true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            hit = true;
        }
    }

    if (hit) {
        rec.t = temp;
        rec.p = r(rec.t);
        Normal outward_normal = (rec.p - center) / radius;
        rec.SetFaceNormal(r, outward_normal);
        rec.material = material;
        return true;
    }

    return false;
}
