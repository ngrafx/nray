#include "scene.h"
#include "parser.h"

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


Color TraceNormal(const Ray& r, shared_ptr<Primitive> world, int depth) {
    Intersection rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Color(0,0,0);

    if (world->Intersect(r, 0.001, Infinity, rec)) {
        Ray scattered;
        Color attenuation;
        // return rec.normal*0.5 + Color(0.5, 0.5, 0.5);
        return rec.normal;
    }

    return Color(0,0,0);
}


bool Scene::_getNextTile(int &tile) {
    std::unique_lock<std::mutex> lck(_mtx);
    if (_tilesToRender.empty())
        return false;
    tile = _tilesToRender.front();
    _tilesToRender.pop();
    return true;
}


void Scene::RenderTile() {
    int tile_number;
    while(_getNextTile(tile_number)) {
        int tsize = _options.tile_size;
        int start_x = (tile_number % _numTilesWidth) * tsize;
        int end_x = start_x + tsize;
        
        int start_y = (tile_number / _numTilesWidth) * tsize;
        int end_y = start_y + tsize;

        for (int y = start_y; y< end_y; y++) {
            // std::cerr << "\rScanlines remaining: " << _img.Height() - 1 - j  << ' ' << std::flush;
            for (int x = start_x; x< end_x; x++) {
                Color color;
                for (int s = 0; s < _options.pixel_samples; ++s) {
                    Float u = (x + Rng::Rand01()) / _img.Width();
                    Float v = 1.0 - (y + Rng::Rand01()) / _img.Height();
                    Ray r = _camera.GetRay(u, v);
                    color += Trace(r, _world, _options.max_ray_depth);
                    // color += TraceNormal(r, _world, _options.max_ray_depth);
                }
                color /= (Float) _options.pixel_samples;

                _img.SetPixel(x, y, color);
            }
        }

        _updateProgress();
    }
}

void Scene::_updateProgress() {
    std::unique_lock<std::mutex> lck(_mtx);
    _renderedTiles++;
    Float perc = _renderedTiles / (Float)_numTiles;
    //std::cout << "Rendered " << perc * 100 << " %\n";
    std::cerr << "\rRendered " << perc * 100 << "% " << std::flush;
}


Image Scene::Render() {

    // Init the _threads
    _threads.clear();
    int nThreads = std::thread::hardware_concurrency();

    std::cout << "Running " << nThreads << " threads\n";
    
    // Slice the image in multiple tiles
    _numTilesWidth = (int) ceil( (Float)_options.image_width / _options.tile_size );
    int _numTilesHeight = (int) ceil( (Float)_options.image_height / _options.tile_size );
    _numTiles = _numTilesWidth * _numTilesHeight;
    _renderedTiles = 0;

    // Adding all the tiles to the queue
    for (int i = 0; i < _numTiles; i++) {
        _tilesToRender.push(i);
    }
    
    // Send each tile to render on a thread
    for (int i = 0; i < nThreads; i++) {
        _threads.emplace_back(std::thread(&Scene::RenderTileL, this));
    }

    // Wait for each Thread to finish
    for (auto &thread : _threads) {
        thread.join();
    }

    // Return the image buffer
    return std::move(_img);
}


Scene::Scene(const Scene& other)
{
    std::cout << "Scene Copy Constructor" << std::endl;

    _camera = other._camera;
    _world = other._world;
    _options = other._options;
    _img = other._img;
    _numTiles = other._numTiles;
    _numTilesWidth = other._numTilesWidth;
    _renderedTiles = 0;

    _threads.clear();
}

Scene::Scene(Scene&& other)
{
    std::cout << "Scene Move Constructor" << std::endl;

    _camera = std::move(other._camera);
    _world = std::move(other._world);
    _options = std::move(other._options);
    _img = std::move(other._img);
    _numTiles = other._numTiles;
    _numTilesWidth = other._numTilesWidth;
    _renderedTiles = 0;

    _threads.clear();
}

Scene& Scene::operator=(const Scene& other)
{
    std::cout << "Scene Copy Assignment Operator" << std::endl;

    if (&other != this) {
        _camera = other._camera;
        _world = other._world;
        _options = other._options;
        _img = other._img;
        _numTiles = other._numTiles;
        _numTilesWidth = other._numTilesWidth;
        _renderedTiles = 0;

        _threads.clear();
    }
    return *this;
}

Scene& Scene::operator=(Scene&& other)
{
    std::cout << "Scene Move Assignment Operator" << std::endl;

    if (&other != this) {
        _camera = std::move(other._camera);
        _world = std::move(other._world);
        _options = std::move(other._options);
        _img = std::move(other._img);
        _numTiles = other._numTiles;
        _numTilesWidth = other._numTilesWidth;
        _renderedTiles = 0;

        _threads.clear();
    }
    return *this;
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

// Generate a scene filled with Spheres
Scene GenerateBunnyScene(Options &opt) {
    Vec3 lookfrom(0,2,10);
    Vec3 lookat(0,1,0);
    Vec3 vup(0,1,0);
    Float dist_to_focus = 10.0;
    Float aperture = 0.1;

    Camera cam(lookfrom, lookat, vup, (Float)20, opt.image_aspect_ratio, aperture, dist_to_focus);

    // shared_ptr<PrimitiveList> world = make_shared<PrimitiveList>();
    PrimitiveList world;
    std::vector<shared_ptr<Primitive>> bunny = loadObjFile("D:\\dev\\nray\\scenes\\objs\\bunny.obj");

    // shared_ptr<LambertianMaterial> mat = make_shared<LambertianMaterial>(Color(0.5, 0.2, 0.5));
    // for (auto &triangle : bunny) {
    //     auto tri = dynamic_cast<Triangle*>(triangle.get());
    //     triangle->material = mat;
    //     std::cout << "Triangle {" << tri->Mesh()->vp[tri->Index()[0]] << ", " << tri->Mesh()->vp[tri->Index()[1]] << ", " << tri->Mesh()->vp[tri->Index()[2]] << "} \n";
    // }
    // world->add(bunny);
    world.add(bunny);
    shared_ptr<BVH> bvh = make_shared<BVH>(world, 0.0, 0.0);

    // return Scene(world, cam, opt);
    return Scene(bvh, cam, opt);
}