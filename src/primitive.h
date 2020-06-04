#pragma once

#include "nray.h"

#include "geometry.h"


struct Intersection {
    float t;
    Point p;
    Normal normal;
    shared_ptr<Material> material;
    bool front_face;

    void SetFaceNormal(const Ray& r, const Normal& outward_normal) {
        front_face = Dot(r.Direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

// Primitive is the base class that rays can intersects with
// Every 'hittable' object needs to inherit that class
class Primitive {
    public:
        virtual bool Intersect(const Ray& r, Float t_min, Float t_max, Intersection& rec) const = 0;
};


class Sphere: public Primitive  {
    public:
        Sphere() {}

        Sphere(Point center_, Float radius_, shared_ptr<Material> mat_)
            : center(center_), radius(radius_), material(mat_) {};

        virtual bool Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const;

        Point center;
        Float radius;
        shared_ptr<Material> material;
};