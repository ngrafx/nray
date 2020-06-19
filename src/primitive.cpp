#include <algorithm>
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


bool PrimitiveList::BoundingBox(Float t0, Float t1, BBox& output_box) const {
    if (_objects.empty())
        return false;

    BBox tmp_box;
    bool first_box = true;

    for (const auto& object : _objects) {
        if (!object->BoundingBox(t0, t1, tmp_box))
            return false;
        output_box = first_box ? tmp_box : BBoxUnion(output_box, tmp_box);
        first_box = false;
    }

    return true;
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

bool Sphere::BoundingBox(Float t0, Float t1, BBox& output_box) const {
    output_box = BBox( center - Vec3(radius, radius, radius),
                       center + Vec3(radius, radius, radius) );
    return true;
}




// BVH Implementation

BVH::BVH( std::vector<shared_ptr<Primitive>>& objects,
         size_t start, size_t end, Float time0, Float time1 ) {
    
    // Randomly choose an axis
    int axis = (int)Rng::RandRange(0, 3);
    auto comparator = (axis==0) ? _BBoxCompareX
                     :(axis==1) ? _BBoxCompareY
                                : _BBoxCompareZ;

    size_t object_span = end-start;
    if (object_span == 1) {
        _left = _right = objects[start];
    }
    else if (object_span == 2) {
        if (comparator(objects[start], objects[start+1])) {
            _left = objects[start];
            _right = objects[start+1];
        } else {
            _left = objects[start+1];
            _right = objects[start];            
        }
    }
    else {
        // Sort the objects according to our chosen axis
        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        auto mid = start + object_span / 2;
        _left = make_shared<BVH> (objects, start, mid, time0, time1);
        _right = make_shared<BVH> (objects, mid, end, time0, time1);
    }

    BBox bbox_left, bbox_right;
    if ( !_left->BoundingBox(time0, time1, bbox_left)
      || !_right->BoundingBox(time0, time1, bbox_right) ) {
          std::cerr << "No BBox in BVH Constructor.\n";
    }

    _bbox = BBoxUnion(bbox_left, bbox_right);

}


inline bool _BBoxCompare(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b, int axis) {
    BBox ba, bb;
    if (!a->BoundingBox(0,0, ba) || !b->BoundingBox(0,0, bb)) {
        std::cerr << "No BoundingBox in BVH Constructor\n";
    }
    return ba.Min()[axis] < ba.Min()[axis];
}

bool _BBoxCompareX(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b) {
    return _BBoxCompare(a, b, 0);
}

bool _BBoxCompareY(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b) {
    return _BBoxCompare(a, b, 1);
}

bool _BBoxCompareZ(const shared_ptr<Primitive> a, const shared_ptr<Primitive> b) {
    return _BBoxCompare(a, b, 2);
}

bool BVH::BoundingBox(Float t0, Float t1, BBox& output_box) const {
    output_box = _bbox;
    return true;
}

bool BVH::Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const {
    // Exit if the r doesn't intersect the bbox
    if (!_bbox.Intersect(r, tmin, tmax))
        return false;

    // Check if we intersect with the left obj
    bool hit_left = _left->Intersect(r, tmin, tmax, rec);
    // Check if we intersect with the right (and the hit is closer than the left one)
    bool hit_right = _right->Intersect(r, tmin, hit_left ? rec.t : tmax, rec);

    return hit_left || hit_right;
}


// Triangle & Triangle Mesh Implementation
Triangle::Triangle(const shared_ptr<TriangleMesh> &mesh, int index, shared_ptr<Material> mat) : _mesh(mesh), material(mat) {
    _index = &mesh->vertexIndices[3*index];
}


bool Triangle::Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const {
    // Get Vertices pos
    const Point &p0 = _mesh->vp[_index[0]];
    const Point &p1 = _mesh->vp[_index[1]];
    const Point &p2 = _mesh->vp[_index[2]];

    Vec3 v0v1 = p1 - p0;
    Vec3 v0v2 = p2 - p0;
    Vec3 pvec = Cross(r.Direction(),v0v2);
    float det = Dot(v0v1, pvec);
#ifdef CULLING
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return false;
#else
//     // ray and triangle are parallel if det is close to 0
//     if (fabs(det) < kEpsilon) return false;
#endif
    float invDet = 1 / det;

    Vec3 tvec = r.Origin() - p0;
    Float u = Dot(tvec, pvec) * invDet;
    if (u < 0 || u > 1) return false;

    Vec3 qvec = Cross(tvec, v0v1);
    Float v = Dot(r.Direction(), qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    
    Float t = Dot(v0v2, qvec) * invDet;
    // Exit is t is not between min and max
    if (t < tmin || t > tmax) return false;

    // Set intersection info
    rec.t = t;
    rec.p = r(rec.t);
    rec.material = material;

    // Compute Normal
    Vec3 outward_normal =  Normalize(Cross(v0v1, v0v2));
    rec.SetFaceNormal(r, outward_normal);

    // const Normal &n0 = _mesh->vn[_index[0]];
    // const Normal &n1 = _mesh->vn[_index[1]];
    // const Normal &n2 = _mesh->vn[_index[2]];
    // Vec3 nn = (n0+n1+n2)/3;
    // rec.SetFaceNormal(r, nn);

    return true;

}

bool Triangle::BoundingBox(Float t0, Float t1, BBox& output_box) const {
    // Get Vertices pos
    const Point &p0 = _mesh->vp[_index[0]];
    const Point &p1 = _mesh->vp[_index[1]];
    const Point &p2 = _mesh->vp[_index[2]];

    // Get min and max value of each axis
    Float minx = Min(Min(p0.x, p1.x), p2.x);
    Float miny = Min(Min(p0.y, p1.y), p2.y);
    Float minz = Min(Min(p0.z, p1.z), p2.z);
    Float maxx = Max(Max(p0.x, p1.x), p2.x);
    Float maxy = Max(Max(p0.y, p1.y), p2.y);
    Float maxz = Max(Max(p0.z, p1.z), p2.z);

    BBox box( Vec3(minx, miny, minz), Vec3(maxx, maxy, maxz));
    output_box = box;

    return true;
}


// Triangle & Triangle Mesh Utilities

// Creates a triangle mesh and returns a vector of Triangle Primitive
// referencing it
std::vector<shared_ptr<Primitive>> CreateTriangleMesh(
    int nTriangles, std::vector<int> &&vertexIndices, 
    std::vector<Point> &&vp, std::vector<Normal> &&vn,
    shared_ptr<Material> material) {

    std::cout << " - Creating TriangleMesh: " << nTriangles << " triangles, " << vp.size() << " vertices\n";

    shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>( nTriangles, std::move(vertexIndices), std::move(vp), std::move(vn) );
    std::vector<shared_ptr<Primitive>> tris;

    // shared_ptr<Material> mat = make_shared<EmissiveMaterial>(Color(1, 0.2, 0.5));
    for (int i = 0; i < nTriangles; ++i){
        tris.emplace_back(make_shared<Triangle>( mesh, i, material));
    }
    return std::move(tris);
}