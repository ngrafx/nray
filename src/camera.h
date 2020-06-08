// #ifndef CAMERA_H
// #define CAMERA_H

#pragma once

#include "nray.h"
#include "geometry.h"

class Camera {
    public:

        Camera(
            Vec3 lookfrom, Vec3 lookat, Vec3 vup,
            Float vfov, // top to bottom, in degrees
            Float aspect, Float aperture, Float focus_dist
        );

        Camera(
            Vec3 lookfrom, Vec3 lookat, Vec3 vup,
            Float vfov, // top to bottom, in degrees
            Float aspect, Float aperture, Float focus_dist, Float t0, Float t1
        );

        Ray GetRay(Float s, Float t) ;

    public:
        Vec3 origin;
        Vec3 lower_left_corner;
        Vec3 horizontal;
        Vec3 vertical;
        Vec3 u, v, w;
        Float lens_radius;
        Float time0, time1;  // shutter open/close times
};

// #endif