#pragma once

#include "primitive.h"


// ImplicitPrimitive Base virtual Class
// Implicit primitive are primitives that are ray marched thanks to their SDF Function
class ImplicitPrimitive: public Primitive  {
    public:

        bool Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const {
            Float t = tmin;
            for(int i=0; i<512; i++) {
                Float h = sdf( r(t) );
                if( h < MachineEpsilon * t) {
                    rec.t = t;
                    rec.p = r(rec.t);
                    // Compute Gradient to get normal
                    Float delta = 10e-5;
                    // Float delta = 0.0001;
                    Normal norm = Normalize(Vec3( sdf(rec.p + Vec3(delta, 0, 0)) - sdf(rec.p + Vec3(-delta, 0, 0)),
                                                  sdf(rec.p + Vec3(0, delta, 0)) - sdf(rec.p + Vec3(0, -delta, 0)),
                                                  sdf(rec.p + Vec3(0, 0, delta)) - sdf(rec.p + Vec3(0, 0, -delta)) ));
                    rec.SetFaceNormal(r, norm);
                    rec.material = this->GetMaterial();
                    return true;
                }
                t += h;
                if (t >= tmax)
                    return false;
            }
            return false;
        };

        virtual bool BoundingBox(Float t0, Float t1, BBox& output_box) const = 0;

        virtual Float sdf(Point p) const = 0;

        virtual shared_ptr<Material> GetMaterial() const = 0;


};

class ImplicitSphere: public ImplicitPrimitive {
    public:
        ImplicitSphere(Point center, Float radius, shared_ptr<Material> mat) : _center(center), _radius(radius), material(mat) {}

        bool BoundingBox(Float t0, Float t1, BBox& output_box) const {
            output_box = BBox( _center - Vec3(_radius, _radius, _radius),
                            _center + Vec3(_radius, _radius, _radius) );
            return true;
        }

        Float sdf(Point p) const {
            return ( p - _center ).Length() - _radius;
        }

        shared_ptr<Material> GetMaterial() const {
            return material;
        }


    shared_ptr<Material> material;

    private:
        Point _center;
        Float _radius;

};


class ImplicitBox: public ImplicitPrimitive {
    public:
        ImplicitBox(Point center, Vec3 size, shared_ptr<Material> mat) : _center(center), _size(size), material(mat) {}

        bool BoundingBox(Float t0, Float t1, BBox& output_box) const {
            output_box = BBox( _center - _size,
                               _center + _size );
            return true;
        }

        Float sdf(Point p) const {
            Vec3 d = Abs(p-_center) - _size ;
            return Min(Max(d.x,Max(d.y,d.z)),0.0) + Vec3(Max(d.x,0),Max(d.y,0),Max(d.z,0)).Length();
        }

        shared_ptr<Material> GetMaterial() const {
            return material;
        }


    shared_ptr<Material> material;

    private:
        Point _center;
        Vec3 _size;
};
