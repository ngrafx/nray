#include "primitive.h"


bool PrimitiveList::Intersect(const Ray& r, Float t_min, Float t_max, Intersection& rec) const {
    Intersection temp_rec;
    auto hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : _objects) {
        if (object->Intersect(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}


// Sphere implementation

bool Sphere::Intersect(const Ray& r, Float t_min, Float t_max, Intersection& rec) const {
    Vec3 oc = r.Origin() - center;
    Float a = r.Direction().LengthSquared();
    Float half_b = Dot(oc, r.Direction());
    Float c = oc.LengthSquared() - radius*radius;
    Float discriminant = half_b*half_b - a*c;


    if (discriminant > 0) {
        Float root = sqrt(discriminant);

        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r(rec.t);
            Vec3 outward_normal = (rec.p - center) / radius;
            rec.SetFaceNormal(r, outward_normal);
            rec.material = material;
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r(rec.t);
            Vec3 outward_normal = (rec.p - center) / radius;
            rec.SetFaceNormal(r, outward_normal);
            rec.material = material;
            return true;
        }
    }
    return false;

    return false;
}
