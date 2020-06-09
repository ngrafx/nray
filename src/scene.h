#pragma once

#include <thread>
#include <mutex>

#include "nray.h"
#include "image.h"
#include "camera.h"
#include "primitive.h"

class Scene {
  public:

    Scene(Options opt) : _options(opt) { _img = Image(_options.image_width, _options.image_height); }
    Scene(shared_ptr<Primitive> world, Camera camera, Options opt) : _world(world), _camera(camera), _options(opt) {
       _img = Image(_options.image_width, _options.image_height);
    }

    void LoadFromFile(char const *filename);

    void SetWorld();

    Image Render();
    void RenderTile(int tile_number);
    void RenderTileDummy(int tile_number);

  private:
    Camera _camera;
    shared_ptr<Primitive> _world;
    Options _options;
    Image _img;
    int _numTiles;

    std::vector<std::thread> _threads;
    std::mutex _mtx;
};

Scene GenerateTestScene(Options &opt);
Color Trace(const Ray& r, shared_ptr<Primitive> world, int depth);