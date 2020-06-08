/*
    nray source code is Copyright(c) 2020
                        Nicolas Delbecq

    This file is part of nray.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */
#include "nray.h"

#include "geometry.h"
#include "rand.h"
#include "primitive.h"
#include "camera.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// Initialize Random Number Generator
// We initialize the generator with a constant seed
// as we need the random number generation to be 
// deterministic
std::uniform_real_distribution<Float> Rng::distribution01 = std::uniform_real_distribution<Float>(0,1);
std::mt19937 Rng::generator = std::mt19937(0);


// Generate a scene filled with Spheres
PrimitiveList RandomScene() {
    PrimitiveList world;

    world.add(
        make_shared<Sphere>(Point(0,-1000,0), 1000, make_shared<LambertianMaterial>(Color(0.5, 0.5, 0.5)))
    );

    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = Rng::Rand01();
            Point center(a + 0.9*Rng::Rand01(), 0.2, b + 0.9*Rng::Rand01());
            if ((center - Vec3(4, 0.2, 0)).Length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = RandomVector<Float>() * RandomVector<Float>();
                    world.add(
                        make_shared<Sphere>(center, 0.2, make_shared<LambertianMaterial>(albedo)));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = RandomVector<Float>(.5, 1);
                    auto fuzz = Rng::RandRange(0, .5);
                    world.add(
                        make_shared<Sphere>(center, 0.2, make_shared<MetalMaterial>(albedo, fuzz)));
                } else {
                    // glass
                    world.add(make_shared<Sphere>(center, 0.2, make_shared<DielectricMaterial>(1.5)));
                }
            }
        }
    }

    world.add(
        make_shared<Sphere>(Point(0, 1, 0), 1.0, make_shared<DielectricMaterial>(1.5)));
    world.add(
        make_shared<Sphere>(Point(-4, 1, 0), 1.0, make_shared<LambertianMaterial>(Color(0.4, 0.2, 0.1))));
    world.add(
        make_shared<Sphere>(Point(4, 1, 0), 1.0, make_shared<MetalMaterial>(Color(0.7, 0.6, 0.5), 0.0)));

    return world;
}


Color trace(const Ray& r, const Primitive& world, int depth) {
    Intersection rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Color(0,0,0);

    if (world.Intersect(r, 0.001, Infinity, rec)) {
        Ray scattered;
        Color attenuation;
        if (rec.material->Scatter(r, rec, attenuation, scattered))
            return attenuation * trace(scattered, world, depth-1);
        return Color(0,0,0);
    }

    // If the ray doesn't intersect with any primitive
    // returns our 'sky' color
    Vec3 unit_direction = Normalize(r.Direction());
    auto t = 0.5*(unit_direction.y + 1.0);
    return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

// int main() {
//     Vec3 v(1.0, 1.2, 3.2);
//     Vec3 v2(1.0, 1.2, 3.2);
//     Vec3 v3 = v * v2 * 2;
//     v3 *= 5;
//     v3 *= v2;
//     Vec3 v4 = v / v2 / 1.0;
//     v4 /= 3;
//     v4 /= v3;
//     Vec3 v5 = Normalize(v4);
//     bool eq = v == v2;
//     bool neq = v != v2;
//     Float d = Distance(v, v2);
//     d = DistanceSquared(v, v2);
//     Float dot = Dot(v, v2);
//     Vec3 v6 = Cross(v, v2);
//     std::cout << "Hello World!" << "\n";
//     std::cout << (Normalize(v) * 10).Length() << std::endl;
//     std::cout << v*v/Normalize(v) << std::endl;
//     return 0;
// }

int main() {
    const int image_width = 200;
    const int image_height = 100;
    const int samples_per_pixel = 10;
    const int max_depth = 15;
    Float aspect_ratio = double(image_width) / image_height;

    // Initialize img buffer
    unsigned char *img;
    int n = image_width * image_height * 3;
    img = new unsigned char[n];

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    PrimitiveList world = RandomScene();

    Vec3 lookfrom(13,2,3);
    Vec3 lookat(0,0,0);
    Vec3 vup(0,1,0);
    Float dist_to_focus = 10.0;
    Float aperture = 0.1;

    Camera cam(lookfrom, lookat, vup, (Float)20, aspect_ratio, aperture, dist_to_focus);

    for (int j = 0; j < image_height; j++) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            Color color;
            for (int s = 0; s < samples_per_pixel; ++s) {
                Float u = (i + Rng::Rand01()) / image_width;
                Float v = 1.0 - (j + Rng::Rand01()) / image_height;
                Ray r = cam.GetRay(u, v);
                color += trace(r, world, max_depth);
            }
            color /= (Float)samples_per_pixel;

            // Remove nans
            if (color[0] != color[0]) color[0] = 0.0;
            if (color[1] != color[1]) color[1] = 0.0;
            if (color[2] != color[2]) color[2] = 0.0;

            // Gamma correction
            color = Vec3( sqrt(color.x), sqrt(color.y), sqrt(color.z));

            // Write to img buffer
            unsigned char ir = static_cast<unsigned char>(256 * Clamp(color.x, 0.0, 0.999));
            unsigned char ig = static_cast<unsigned char>(256 * Clamp(color.y, 0.0, 0.999));
            unsigned char ib = static_cast<unsigned char>(256 * Clamp(color.z, 0.0, 0.999));

            int index = (i + j * image_width) * 3;

            img[index + 0] = ir;
            img[index + 1] = ig;
            img[index + 2] = ib;
        }
    }

    stbi_write_png("D:\\dev\\nray\\render2.png", image_width, image_height, 3, img, 0);

    delete[] img;
    std::cerr << "\nDone.\n";
}