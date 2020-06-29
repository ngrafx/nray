
#include "camera.h"

Camera::Camera(
    Vec3 lookfrom, Vec3 lookat, Vec3 vup,
    Float vfov, // top to bottom, in degrees
    Float aspect, Float aperture, Float focus_dist,
    bool dof
) {
    *this = Camera(lookfrom, lookat, vup, vfov, aspect, aperture, focus_dist, dof, 0, 0);
}

Camera::Camera(
    Vec3 lookfrom, Vec3 lookat, Vec3 vup,
    Float vfov, // top to bottom, in degrees
    Float aspect, Float aperture, Float focus_dist, bool dof, Float t0, Float t1
) {
    origin = lookfrom;
    lens_radius = aperture / 2;
    time0 = t0;
    time1 = t1;
    do_dof = dof;

    auto theta = Radians(vfov);
    auto half_height = tan(theta/2);
    auto half_width = aspect * half_height;

    w = Normalize(lookfrom - lookat);
    u = Normalize(Cross(vup, w));
    v = Cross(w, u);

    lower_left_corner = origin
                        - half_width*focus_dist*u
                        - half_height*focus_dist*v
                        - focus_dist*w;

    horizontal = 2*half_width*focus_dist*u;
    vertical = 2*half_height*focus_dist*v;
}

Ray Camera::GetRay(Float s, Float t) {
    Vec3 rd;
    if (do_dof)
        rd = lens_radius * RandomInUnitDisk<Float>();
    Vec3 offset = u * rd.x + v * rd.y;
    return Ray(
        origin + offset,
        lower_left_corner + s*horizontal + t*vertical - origin - offset,
        RayType::Primary,
        Rng::RandRange(time0, time1)
    );
}