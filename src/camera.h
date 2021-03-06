#pragma once

#include "nray.h"
#include "geometry.h"


// Camera Class
// Used to shoot rays through the scene from given pixel.
// It represents our 'eye'
class Camera {
    public:

        Camera() {}

        Camera(
            Vec3 lookfrom, Vec3 lookat, Vec3 vup,
            Float vfov, // top to bottom, in degrees
            Float aspect, Float aperture, Float focus_dist,
            bool do_dof
        );

        Camera(
            Vec3 lookfrom, Vec3 lookat, Vec3 vup,
            Float vfov, // top to bottom, in degrees
            Float aspect, Float aperture, Float focus_dist,
            bool do_dof, Float t0, Float t1
        );

        Ray GetRay(Float s, Float t) ;

    public:
        Vec3 origin;
        Vec3 lower_left_corner;
        Vec3 horizontal;
        Vec3 vertical;
        Vec3 u, v, w;
        Float lens_radius{1};
        Float time0{0};
        Float time1{0};  // shutter open/close times
        bool do_dof;
};
