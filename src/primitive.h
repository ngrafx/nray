#pragma once

#include <vector>

#include "nray.h"

#include "geometry.h"
#include "material.h"

/* Interesction stores all the data related to 
a Ray hitting (intersecting) a surface (Primitive)
*/
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
        // Intersect is a pure virtual function that every Primitive needs to implement.
        // It does not really matter how the Primitive is implemented as long as it 
        // can be intersected
        virtual bool Intersect(const Ray& r, Float t_min, Float t_max, Intersection& rec) const = 0;
};

// Primitive List Container
class PrimitiveList: public Primitive  {
    public:
        PrimitiveList() {}
        PrimitiveList(shared_ptr<Primitive> object) { add(object); }

        void clear() { _objects.clear(); }
        void add(shared_ptr<Primitive> object) { _objects.push_back(object); }

        virtual bool Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const;

    private:
        std::vector<shared_ptr<Primitive>> _objects;
};

// Sphere Primitive
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
