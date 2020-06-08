#include "scene.h"


Color Trace(const Ray& r, shared_ptr<Primitive> world, int depth) {
    Intersection rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Color(0,0,0);

    if (world->Intersect(r, 0.001, Infinity, rec)) {
        Ray scattered;
        Color attenuation;
        if (rec.material->Scatter(r, rec, attenuation, scattered))
            return attenuation * Trace(scattered, world, depth-1);
        return Color(0,0,0);
    }

    // If the ray doesn't intersect with any primitive
    // returns our 'sky' color
    Vec3 unit_direction = Normalize(r.Direction());
    auto t = 0.5*(unit_direction.y + 1.0);
    return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

void Scene::Render(Image &img) {

    for (int j = 0; j < img.Height(); j++) {
        std::cerr << "\rScanlines remaining: " << img.Height() - 1 - j  << ' ' << std::flush;
        for (int i = 0; i < img.Width(); ++i) {
            Color color;
            for (int s = 0; s < _options.pixel_samples; ++s) {
                Float u = (i + Rng::Rand01()) / img.Width();
                Float v = 1.0 - (j + Rng::Rand01()) / img.Height();
                Ray r = _camera.GetRay(u, v);
                color += Trace(r, _world, _options.max_ray_depth);
            }
            color /= (Float) _options.pixel_samples;

            img.SetPixel(i, j, color);
        }
    }
}


// Generate a scene filled with Spheres
Scene GenerateTestScene(Options &opt) {
    Vec3 lookfrom(13,2,3);
    Vec3 lookat(0,0,0);
    Vec3 vup(0,1,0);
    Float dist_to_focus = 10.0;
    Float aperture = 0.1;

    Camera cam(lookfrom, lookat, vup, (Float)20, opt.image_aspect_ratio, aperture, dist_to_focus);

    shared_ptr<PrimitiveList> world = make_shared<PrimitiveList>();

    world->add(
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
                    world->add(
                        make_shared<Sphere>(center, 0.2, make_shared<LambertianMaterial>(albedo)));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = RandomVector<Float>(.5, 1);
                    auto fuzz = Rng::RandRange(0, .5);
                    world->add(
                        make_shared<Sphere>(center, 0.2, make_shared<MetalMaterial>(albedo, fuzz)));
                } else {
                    // glass
                    world->add(make_shared<Sphere>(center, 0.2, make_shared<DielectricMaterial>(1.5)));
                }
            }
        }
    }

    world->add(
        make_shared<Sphere>(Point(0, 1, 0), 1.0, make_shared<DielectricMaterial>(1.5)));
    world->add(
        make_shared<Sphere>(Point(-4, 1, 0), 1.0, make_shared<LambertianMaterial>(Color(0.4, 0.2, 0.1))));
    world->add(
        make_shared<Sphere>(Point(4, 1, 0), 1.0, make_shared<MetalMaterial>(Color(0.7, 0.6, 0.5), 0.0)));

    return Scene(world, cam, opt);
}