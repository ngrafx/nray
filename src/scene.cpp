

#include "scene.h"
#include "parser.h"

#include "image.h"
#include "implicit.h"

Color Trace(const Ray& r, Scene *scene, int depth) {

    // If we've exceeded the ray bounce limit, no more light is gathered.
    // if (depth <= 0) {
    //     // If we're color at Ray Limit
    //     if (scene->Settings().useBgColorAtLimit)
    //         return scene->SampleEnvironment(r);
    //     return Color(0,0,0);
    // }

    // Check if we've exceeded the max ray depth
    int max = 0;
    switch (r.Type()) {
        case RayType::Primary :
            max = 999999;
            break;
        case RayType::Diffuse :
            max = scene->Settings().max_diffuse_rdepth;
            break;
        case RayType::Reflect :
            max = scene->Settings().max_reflect_rdepth;
            break;
        case RayType::Refract :
            max = scene->Settings().max_refract_rdepth;
            break;
    }
    if (depth > max) {
        // If we're color at Ray Limit
        if (scene->Settings().useBgColorAtLimit)
            return scene->SampleEnvironment(r);
        return Color(0,0,0);
    }

    Intersection rec;
    // If no intersection is found return the environment color
    if (!scene->World()->Intersect(r, 0.001, Infinity, rec)) {
        return scene->SampleEnvironment(r);
    }

    // Scatter light
    Ray scattered;
    Color attenuation;
    Color emitted = rec.material->Emitted();
    if (!rec.material->Scatter(r, rec, attenuation, scattered))
        return emitted;
    // return emitted + attenuation * Trace(scattered, scene, depth-1);
    return emitted + attenuation * Trace(scattered, scene, depth+1);
}

Color TraceNormalOnly(const Ray& r, Scene *scene) {
    Intersection rec;
    if (scene->World()->Intersect(r, 0.001, Infinity, rec)) {
        Ray scattered;
        Color attenuation;
        // Remap normals between 0 and 1
        return rec.normal*0.5 + Color(0.5, 0.5, 0.5);
        // return rec.normal;
    }
    return Color(0,0,0);
}


Scene::Scene(const Scene& other) {
    _camera = other._camera;
    _world = other._world;
    _options = other._options;
    _img = other._img;
    ibl = other.ibl;
}
Scene::Scene(Scene&& other) {
    _camera = other._camera;
    _world = std::move(other._world);
    _options = other._options;
    _img = std::move(other._img);
    ibl = std::move(other.ibl);
}
Scene& Scene::operator=(const Scene& other) {
    _camera = other._camera;
    _world = other._world;
    _options = other._options;
    _img = other._img;
    ibl = other.ibl;
    return *this;
}
Scene& Scene::operator=(Scene&& other) {
    _camera = other._camera;
    _world = std::move(other._world);
    _options = other._options;
    _img = std::move(other._img);
    ibl = std::move(other.ibl);
    return *this;
}


bool Scene::_getNextTile(int &tile) {
    // Returns true if a tile was given
    // false if there's no more tile in the queue
    std::unique_lock<std::mutex> lck(_mtx);
    if (_tilesToRender.empty())
        return false;
    tile = _tilesToRender.front();
    _tilesToRender.pop();
    return true;
}


void Scene::_RenderTile() {
    int tile_number;
    // Run until there's no more tiles left to render
    while(_getNextTile(tile_number)) {
        // Get the start & end pixel position of the tile
        int tsize = _options.tile_size;
        int start_x = (tile_number % _numTilesWidth) * tsize;
        int end_x = start_x + tsize;
        
        int start_y = (tile_number / _numTilesWidth) * tsize;
        int end_y = start_y + tsize;

        // For every pixel in the tile
        for (int y = start_y; y< end_y; y++) {
            for (int x = start_x; x< end_x; x++) {
                Color color;
                // For every sample
                for (int s = 0; s < _options.pixel_samples; ++s) {
                    Float u = (x + Rng::Rand01()) / _img.Width();
                    Float v = 1.0 - (y + Rng::Rand01()) / _img.Height();
                    Ray r = _camera.GetRay(u, v);
                    if(_options.normalOnly){
                        color += TraceNormalOnly(r, this);
                    } else {
                        // color += ClampMax(Trace(r, this, _options.max_ray_depth), _options.color_limit);
                        color += ClampMax(Trace(r, this, 0), _options.color_limit);
                    }
                }
                color /= (Float) _options.pixel_samples;

                _img.SetPixel(x, y, color);
            }
        }
        _updateProgress();
    }
}

void Scene::_updateProgress() {
    // Update the number of tile rendered
    // and report progress to the user
    std::unique_lock<std::mutex> lck(_mtx_cout);
    _renderedTiles++;
    Float perc = _renderedTiles / (Float)_numTiles;
    //std::cout << "Rendered " << perc * 100 << " %\n";
    std::cerr << "\rRendered " << perc * 100 << "% " << std::flush;
}


Image Scene::Render() {

    // Initialize the image buffer
    _img = Image(_options.image_width, _options.image_height);

    // Init the _threads
    _threads.clear();
    
    // Slice the image in multiple tiles
    _numTilesWidth = (int) ceil( (Float)_options.image_width / _options.tile_size );
    int _numTilesHeight = (int) ceil( (Float)_options.image_height / _options.tile_size );
    _numTiles = _numTilesWidth * _numTilesHeight;
    _renderedTiles = 0;

    // Get the number of threads available
    int availableThreads = std::thread::hardware_concurrency();
    // If user override the thread number
    if (_options.max_threads != -1)
        availableThreads = Min(availableThreads, _options.max_threads);

    // Final number of threads
    int nThreads = Min(_numTiles, availableThreads);
    std::cout << "\n\nRunning " << nThreads << " threads\n";


    // Adding all the tiles to the queue
    for (int i = 0; i < _numTiles; i++) {
        _tilesToRender.push(i);
    }
    
    // Send each tile to render on a thread
    for (int i = 0; i < nThreads; i++) {
        _threads.emplace_back(std::thread(&Scene::_RenderTile, this));
    }

    // Wait for each Thread to finish
    for (auto &thread : _threads) {
        thread.join();
    }

    // Return the image buffer
    return std::move(_img);
}

void Scene::PrintSettings() {
    std::cout << "\nRender Settings: \n";
    std::cout << "Image: " << _options.image_width << "x" << _options.image_height << "\n";
    std::cout << "Tile size: " << _options.tile_size << "x" << _options.tile_size << "\n";
    std::cout << "Pixel samples: " << _options.pixel_samples << "\n";
    std::cout << "Diffuse Ray Depth: " << _options.max_diffuse_rdepth << "\n";
    std::cout << "Reflect Ray Depth: " << _options.max_reflect_rdepth << "\n";
    std::cout << "Refract Ray Depth: " << _options.max_refract_rdepth << "\n";
    std::cout << "Color Limit: " << _options.color_limit << "\n";
    std::cout << "Output: " << _options.image_out << "\n\n";
    if(_options.normalOnly)
        std::cout << "\nSetting renderer to Normal Only\n\n";
}


// Generate a scene filled with Spheres
Scene GenerateTestScene(RenderSettings opt) {
    Vec3 lookfrom(13,2,3);
    // Vec3 lookfrom(0,0,10);
    Vec3 lookat(0,0,0);
    Vec3 vup(0,1,0);
    Float dist_to_focus = 10.0;
    Float aperture = 0.1;

    Camera cam(lookfrom, lookat, vup, (Float)20, opt.image_aspect_ratio, aperture, dist_to_focus, true);

    // shared_ptr<PrimitiveList> world = make_shared<PrimitiveList>();
    PrimitiveList world;

    world.add(
        make_shared<Sphere>(Point(0,-1000,0), 1000, make_shared<LambertianMaterial>(Color(0.5, 0.5, 0.5)))
    );

    // world.add(
    //     make_shared<ImplicitPlane>(0, make_shared<LambertianMaterial>(Color(0.5, 0.5, 0.5)))
    // );

    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = Rng::Rand01();
            Point center(a + 0.9*Rng::Rand01(), 0.2, b + 0.9*Rng::Rand01());
            if ((center - Vec3(4, 0.2, 0)).Length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = RandomVector<Float>() * RandomVector<Float>();
                    if (Rng::Rand01() < 0.3) {
                        world.add(
                            make_shared<ImplicitBox>(center, Vec3(0.1, 0.35, 0.2), make_shared<LambertianMaterial>(albedo))); 
                    } else {
                        world.add(
                            make_shared<Sphere>(center, 0.2, make_shared<LambertianMaterial>(albedo)));
                    }
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
        make_shared<ImplicitSphere>(Point(-4, 1, 0), 1.0, make_shared<EmissiveMaterial>(Color(5, 0.2, 0.1))));
    world.add(   
        make_shared<Sphere>(Point(4, 1, 0), 1.0, make_shared<MetalMaterial>(Color(0.7, 0.6, 0.5), 0.0)));

    shared_ptr<BVH> bvh = make_shared<BVH>(world, 0.0, 0.0);
    Scene scene(bvh, cam, opt);
    // Scene scene(sph, cam, opt);
    scene.ibl.LoadFromFile("../scenes/maps/abandoned_hopper_terminal_02_2k.hdr");
    return std::move(scene);
}