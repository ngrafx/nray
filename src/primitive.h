#pragma once

#include <vector>

#include "nray.h"

#include "geometry.h"
#include "material.h"
#include "bbox.h"

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
        // Intersect and BoundingBox are pure virtual functions that every Primitive needs to implement.
        // It does not really matter how the Primitive is implemented as long as it 
        // can be intersected and we can get its bounding box
        virtual bool Intersect(const Ray& r, Float t_min, Float t_max, Intersection& rec) const = 0;
        virtual bool BoundingBox(Float t0, Float t1, BBox& output_box) const = 0;
};

// Primitive List Container
class PrimitiveList: public Primitive  {
    public:
        PrimitiveList() {}
        PrimitiveList(shared_ptr<Primitive> object) { add(object); }

        void clear() { _objects.clear(); }
        void add(shared_ptr<Primitive> object) { _objects.push_back(object); }

        virtual bool Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const;
        virtual bool BoundingBox(Float t0, Float t1, BBox& output_box) const;

    private:
        std::vector<shared_ptr<Primitive>> _objects;
        friend class BVH;
};


// Sphere Primitive
class Sphere: public Primitive  {
    public:
        Sphere() {}

        Sphere(Point center_, Float radius_, shared_ptr<Material> mat_)
            : center(center_), radius(radius_), material(mat_) {};

        virtual bool Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const;
        virtual bool BoundingBox(Float t0, Float t1, BBox& output_box) const;

        Point center;
        Float radius;
        shared_ptr<Material> material;
};


// BVH Container
class BVH : public PrimitiveList {
    public:
        BVH();
        BVH(PrimitiveList& list, Float time0, Float time1) : BVH(list._objects, 0, list._objects.size(), time0, time1) {}

        BVH( std::vector<shared_ptr<Primitive>>& objects,
             size_t start, size_t end, Float time0, Float time1 );

        virtual bool Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const;
        virtual bool BoundingBox(Float t0, Float t1, BBox& output_box) const;
    
    private:

        shared_ptr<Primitive> _left;
        shared_ptr<Primitive> _right;
        BBox _bbox;
};

// BVH Utility functions
inline bool _BBoxCompare(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b, int axis);
bool _BBoxCompareX(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b);
bool _BBoxCompareY(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b);
bool _BBoxCompareZ(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b);


// Triangle & TriangleMesh
struct TriangleMesh {
    TriangleMesh(int nTriangles_, std::vector<int> &&vertexIndices_, unique_ptr<Point[]> &&vp_, unique_ptr<Normal[]> &&vn_ ) : 
                                  nTriangles(nTriangles_), vertexIndices(vertexIndices_) {
        vp = std::move(vp_);
        vn = std::move(vn_);
    }

    const int nTriangles;
    // const int nVertices;
    std::vector<int> vertexIndices;
    unique_ptr<Point[]> vp;
    unique_ptr<Normal[]> vn;

};

class Triangle : public Primitive {
    public:
        Triangle(const shared_ptr<TriangleMesh> &mesh, int index) : _mesh(mesh) { _index = &mesh->vertexIndices[3*index]; }

        virtual bool Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const;
        virtual bool BoundingBox(Float t0, Float t1, BBox& output_box) const;
    private:
        shared_ptr<TriangleMesh> _mesh;
        const int *_index;
};

// Triangle & TriangleMesh Utility Functions
std::vector<shared_ptr<Primitive>> CreateTriangleMesh(
    int nTriangles, std::vector<int> &&vertexIndices, 
    unique_ptr<Point[]> &&vp, unique_ptr<Normal[]> &&vn );