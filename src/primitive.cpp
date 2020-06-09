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
    // Exit if the ray doesn't intersect the bbox
    if (!_bbox.Intersect(r, tmin, tmax))
        return false;

    // Check if we intersect with the left obj
    bool hit_left = _left->Intersect(r, tmin, tmax, rec);
    // Check if we intersect with the right (and the hit is closer than the left one)
    bool hit_right = _right->Intersect(r, tmin, hit_left ? rec.t : tmax, rec);

    return hit_left || hit_right;
}
