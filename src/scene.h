#pragma once

#include "nray.h"
#include "image.h"
#include "camera.h"
#include "primitive.h"

class Scene {
  public:

    Scene();
    Scene(shared_ptr<Primitive> world, Camera camera, Options opt) : _world(world), _camera(camera), _options(opt) {}

    void LoadFromFile(char const *filename);

    void SetWorld();

    void Render(Image &img);

  private:
    Camera _camera;
    shared_ptr<Primitive> _world;
    Options _options;
};

Scene GenerateTestScene(Options &opt);
Color Trace(const Ray& r, shared_ptr<Primitive> world, int depth);