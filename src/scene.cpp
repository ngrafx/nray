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

// Image Scene::Render() {

//     for (int j = 0; j < _img.Height(); j++) {
//         std::cerr << "\rScanlines remaining: " << _img.Height() - 1 - j  << ' ' << std::flush;
//         for (int i = 0; i < _img.Width(); ++i) {
//             Color color;
//             for (int s = 0; s < _options.pixel_samples; ++s) {
//                 Float u = (i + Rng::Rand01()) / _img.Width();
//                 Float v = 1.0 - (j + Rng::Rand01()) / _img.Height();
//                 Ray r = _camera.GetRay(u, v);
//                 color += Trace(r, _world, _options.max_ray_depth);
//             }
//             color /= (Float) _options.pixel_samples;

//             _img.SetPixel(i, j, color);
//         }
//     }
//     return std::move(_img);
// }

void Scene::RenderTileDummy(int tile_number) {
    std::unique_lock<std::mutex> lck(_mtx);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "tile " << tile_number << " finished on thread " << std::this_thread::get_id() << "\n";
}

void Scene::RenderTile(int tile_number) {
    Color rand = RandomVector<Float>();
    int start_x = tile_number * _options.tile_size; 
    int start_y;
    
}

Image Scene::Render() {
    // Init the _threads
    _threads.clear();
    // Slice the image in multiple tiles
    _numTiles = _options.image_width / _options.tile_size;
    // Send each tile to render on a thread
    for (int i = 0; i < _numTiles; i++) {
        _threads.emplace_back(std::thread(&Scene::RenderTileDummy, this, i));
    }

    // Wait for each Thread to finish
    for (auto &thread : _threads) {
        thread.join();
    }

    // Return the image buffer
    return std::move(_img);
}

// Generate a scene filled with Spheres
Scene GenerateTestScene(Options &opt) {
    Vec3 lookfrom(13,2,3);
    Vec3 lookat(0,0,0);
    Vec3 vup(0,1,0);
    Float dist_to_focus = 10.0;
    Float aperture = 0.1;

    Camera cam(lookfrom, lookat, vup, (Float)20, opt.image_aspect_ratio, aperture, dist_to_focus);

    //shared_ptr<PrimitiveList> world = make_shared<PrimitiveList>();
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

    shared_ptr<BVH> bvh = make_shared<BVH>(world, 0.0, 0.0);

    return Scene(bvh, cam, opt);
}