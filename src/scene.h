#pragma once

#include <thread>
#include <mutex>
#include <queue>

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

    Scene(const Scene& other); // copy constructor
    Scene(Scene&& other); // move constructor
    Scene& operator=(const Scene& other); // copy assignment operator
    Scene& operator=(Scene&& other); // move assignment operator

    void LoadFromFile(char const *filename);

    void SetWorld();
    shared_ptr<Primitive> World() { return _world;}

    Image Render();
    void RenderTile();

  private:

    void _updateProgress();
    bool _getNextTile(int &tile);

    Camera _camera;
    shared_ptr<Primitive> _world;
    Options _options;
    Image _img;
    int _numTiles;
    int _numTilesWidth;

    int _renderedTiles;
    std::queue<int> _tilesToRender;

    std::vector<std::thread> _threads;
    std::mutex _mtx;
};

Scene GenerateTestScene(Options &opt);
Scene GenerateBunnyScene(Options &opt);

Color Trace(const Ray& r, shared_ptr<Primitive> world, int depth);