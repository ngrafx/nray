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
bool Triangle::Intersect(const Ray& r, Float tmin, Float tmax, Intersection& rec) const {
    // Get Vertices pos
    const Point &p0 = _mesh->vp[_index[0]];
    const Point &p1 = _mesh->vp[_index[1]];
    const Point &p2 = _mesh->vp[_index[2]];

    // Perform r--triangle intersection test

    // Transform triangle vertices to r coordinate space

    // Translate vertices based on r origin
    Point p0t = p0 - r.Origin();
    Point p1t = p1 - r.Origin();
    Point p2t = p2 - r.Origin();

    // Permute components of triangle vertices and r direction
    int kz = MaxDimension(Abs(r.Direction()));
    int kx = kz + 1;
    if (kx == 3) kx = 0;
    int ky = kx + 1;
    if (ky == 3) ky = 0;
    Vec3 d = Permute(r.Direction(), kx, ky, kz);
    p0t = Permute(p0t, kx, ky, kz);
    p1t = Permute(p1t, kx, ky, kz);
    p2t = Permute(p2t, kx, ky, kz);

    // Apply shear transformation to translated vertex positions
    Float Sx = -d.x / d.z;
    Float Sy = -d.y / d.z;
    Float Sz = 1.f / d.z;
    p0t.x += Sx * p0t.z;
    p0t.y += Sy * p0t.z;
    p1t.x += Sx * p1t.z;
    p1t.y += Sy * p1t.z;
    p2t.x += Sx * p2t.z;
    p2t.y += Sy * p2t.z;

    // Compute edge function coefficients _e0_, _e1_, and _e2_
    Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

    // Fall back to double precision test at triangle edges
    if (sizeof(Float) == sizeof(float) &&
        (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f)) {
        double p2txp1ty = (double)p2t.x * (double)p1t.y;
        double p2typ1tx = (double)p2t.y * (double)p1t.x;
        e0 = (float)(p2typ1tx - p2txp1ty);
        double p0txp2ty = (double)p0t.x * (double)p2t.y;
        double p0typ2tx = (double)p0t.y * (double)p2t.x;
        e1 = (float)(p0typ2tx - p0txp2ty);
        double p1txp0ty = (double)p1t.x * (double)p0t.y;
        double p1typ0tx = (double)p1t.y * (double)p0t.x;
        e2 = (float)(p1typ0tx - p1txp0ty);
    }

    // Perform triangle edge and determinant tests
    if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
        return false;
    Float det = e0 + e1 + e2;
    if (det == 0) return false;

    // Compute scaled hit distance to triangle and test against r $t$ range
    p0t.z *= Sz;
    p1t.z *= Sz;
    p2t.z *= Sz;
    Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    if (det < 0 && (tScaled >= 0 || tScaled < r.TMax() * det))
        return false;
    else if (det > 0 && (tScaled <= 0 || tScaled > r.TMax() * det))
        return false;

    // Compute barycentric coordinates and $t$ value for triangle intersection
    Float invDet = 1 / det;
    Float b0 = e0 * invDet;
    Float b1 = e1 * invDet;
    Float b2 = e2 * invDet;
    Float t = tScaled * invDet;

    // Ensure that computed triangle $t$ is conservatively greater than zero

    // Compute $\delta_z$ term for triangle $t$ error bounds
    Float maxZt = MaxComponent(Abs(Vec3(p0t.z, p1t.z, p2t.z)));
    Float deltaZ = Gamma(3) * maxZt;

    // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
    Float maxXt = MaxComponent(Abs(Vec3(p0t.x, p1t.x, p2t.x)));
    Float maxYt = MaxComponent(Abs(Vec3(p0t.y, p1t.y, p2t.y)));
    Float deltaX = Gamma(5) * (maxXt + maxZt);
    Float deltaY = Gamma(5) * (maxYt + maxZt);

    // Compute $\delta_e$ term for triangle $t$ error bounds
    Float deltaE =
        2 * (Gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

    // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
    Float maxE = MaxComponent(Abs(Vec3(e0, e1, e2)));
    Float deltaT = 3 *
                   (Gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) *
                   std::abs(invDet);
    if (t <= deltaT) return false;

    // Compute triangle partial derivatives
    Vec3 dpdu, dpdv;
    Point2f uv[3];
    GetUVs(uv);

    // Compute deltas for triangle partial derivatives
    Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
    Vec3 dp02 = p0 - p2, dp12 = p1 - p2;
    Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
    bool degenerateUV = std::abs(determinant) < 1e-8;
    if (!degenerateUV) {
        Float invdet = 1 / determinant;
        dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
        dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
    }
    if (degenerateUV || Cross(dpdu, dpdv).LengthSquared() == 0) {
        // Handle zero determinant for triangle partial derivative matrix
        Vec3 ng = Cross(p2 - p0, p1 - p0);
        if (ng.LengthSquared() == 0)
            // The triangle is actually degenerate; the intersection is
            // bogus.
            return false;

        CoordinateSystem(Normalize(ng), &dpdu, &dpdv);
    }

    // Compute error bounds for triangle intersection
    Float xAbsSum =
        (std::abs(b0 * p0.x) + std::abs(b1 * p1.x) + std::abs(b2 * p2.x));
    Float yAbsSum =
        (std::abs(b0 * p0.y) + std::abs(b1 * p1.y) + std::abs(b2 * p2.y));
    Float zAbsSum =
        (std::abs(b0 * p0.z) + std::abs(b1 * p1.z) + std::abs(b2 * p2.z));
    Vec3 pError = Gamma(7) * Vec3(xAbsSum, yAbsSum, zAbsSum);

    // Interpolate $(u,v)$ parametric coordinates and hit point
    Point pHit = b0 * p0 + b1 * p1 + b2 * p2;
    Point2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

    // Test intersection against alpha texture, if present
    if (testAlphaTexture && mesh->alphaMask) {
        SurfaceInteraction isectLocal(pHit, Vec3(0, 0, 0), uvHit, -r.Direction(),
                                      dpdu, dpdv, Normal(0, 0, 0),
                                      Normal(0, 0, 0), r.Time(), this);
        if (mesh->alphaMask->Evaluate(isectLocal) == 0) return false;
    }

    // Fill in _SurfaceInteraction_ from triangle hit
    rec = Intersection(pHit, pError, uvHit, -r.Direction(), dpdu, dpdv,
                                Normal3f(0, 0, 0), Normal3f(0, 0, 0), r.Time(),
                                this, faceIndex);

    // Override surface normal in _isect_ for triangle
    isect->n = isect->shading.n = Normal3f(Normalize(Cross(dp02, dp12)));
    if (reverseOrientation ^ transformSwapsHandedness)
        isect->n = isect->shading.n = -isect->n;

    if (mesh->n || mesh->s) {
        // Initialize _Triangle_ shading geometry

        // Compute shading normal _ns_ for triangle
        Normal ns;
        if (mesh->n) {
            ns = (b0 * mesh->n[v[0]] + b1 * mesh->n[v[1]] + b2 * mesh->n[v[2]]);
            if (ns.LengthSquared() > 0)
                ns = Normalize(ns);
            else
                ns = isect->n;
        } else
            ns = isect->n;

        // Compute shading tangent _ss_ for triangle
        Vec3 ss;
        if (mesh->s) {
            ss = (b0 * mesh->s[v[0]] + b1 * mesh->s[v[1]] + b2 * mesh->s[v[2]]);
            if (ss.LengthSquared() > 0)
                ss = Normalize(ss);
            else
                ss = Normalize(isect->dpdu);
        } else
            ss = Normalize(isect->dpdu);

        // Compute shading bitangent _ts_ for triangle and adjust _ss_
        Vec3 ts = Cross(ss, ns);
        if (ts.LengthSquared() > 0.f) {
            ts = Normalize(ts);
            ss = Cross(ts, ns);
        } else
            CoordinateSystem(ns, &ss, &ts);

        // Compute $\dndu$ and $\dndv$ for triangle shading geometry
        Normal dndu, dndv;
        if (mesh->n) {
            // Compute deltas for triangle partial derivatives of normal
            Vector2f duv02 = uv[0] - uv[2];
            Vector2f duv12 = uv[1] - uv[2];
            Normal dn1 = mesh->n[v[0]] - mesh->n[v[2]];
            Normal dn2 = mesh->n[v[1]] - mesh->n[v[2]];
            Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
            bool degenerateUV = std::abs(determinant) < 1e-8;
            if (degenerateUV) {
                // We can still compute dndu and dndv, with respect to the
                // same arbitrary coordinate system we use to compute dpdu
                // and dpdv when this happens. It's important to do this
                // (rather than giving up) so that r differentials for
                // rs reflected from triangles with degenerate
                // parameterizations are still reasonable.
                Vec3 dn = Cross(Vec3(mesh->n[v[2]] - mesh->n[v[0]]),
                                    Vec3(mesh->n[v[1]] - mesh->n[v[0]]));
                if (dn.LengthSquared() == 0)
                    dndu = dndv = Normal(0, 0, 0);
                else {
                    Vec3 dnu, dnv;
                    CoordinateSystem(dn, &dnu, &dnv);
                    dndu = Normal(dnu);
                    dndv = Normal(dnv);
                }
            } else {
                Float invDet = 1 / determinant;
                dndu = (duv12[1] * dn1 - duv02[1] * dn2) * invDet;
                dndv = (-duv12[0] * dn1 + duv02[0] * dn2) * invDet;
            }
        } else
            dndu = dndv = Normal(0, 0, 0);
        if (reverseOrientation) ts = -ts;
        isect->SetShadingGeometry(ss, ts, dndu, dndv, true);
    }

    // *tHit = t;
    // ++nHits;
    return true;
}
}


bool Triangle::BoundingBox(Float t0, Float t1, BBox& output_box) const {
    // Get Vertices pos
    const Point &p0 = _mesh->vp[_index[0]];
    const Point &p1 = _mesh->vp[_index[1]];
    const Point &p2 = _mesh->vp[_index[2]];

    BBox b0(p0, p1);
    BBox b1(p2,p2);
    output_box =  BBoxUnion( b0, b1 );

    return true;
}


// Triangle & Triangle Mesh Utilities
std::vector<shared_ptr<Primitive>> CreateTriangleMesh(
    int nTriangles, std::vector<int> &&vertexIndices, 
    unique_ptr<Point[]> &&vp, unique_ptr<Normal[]> &&vn ) {

    std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>( nTriangles, std::move(vertexIndices), std::move(vp), std::move(vn) );
    std::vector<std::shared_ptr<Primitive>> tris;

    for (int i = 0; i < nTriangles; ++i)
        tris.emplace_back(std::make_shared<Triangle>( mesh, i));
    return tris;
}