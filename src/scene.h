#pragma once

#include <thread>
#include <mutex>
#include <queue>

#include "nray.h"
#include "image.h"
#include "camera.h"
#include "primitive.h"


// Scene class
// This is the main object responsible for collecting
// Primitive objects and rendering them

class Scene {
  public:
    Scene() {};
    Scene(RenderSettings opt) : _options(opt) {}
    Scene(shared_ptr<Primitive> world, Camera camera, RenderSettings opt) : _world(world), _camera(camera), _options(opt) {}
    Scene(shared_ptr<Primitive> world, Camera camera, RenderSettings opt, Image &&ibl_) : _world(world), _camera(camera), _options(opt), ibl(ibl_) {}

    Scene(const Scene& other); // copy constructor
    Scene(Scene&& other); // move constructor
    Scene& operator=(const Scene& other); // copy assignment operator
    Scene& operator=(Scene&& other); // move assignment operator
    ~Scene() {}

    // void LoadFromFile(char const *filename);

    void SetWorld();
    shared_ptr<Primitive> World() { return _world;}

    Image Render();
    void RenderTile();

    Color SampleEnvironment(const Ray &r) {
      if (ibl.Valid()) {
        Vec3 w = Normalize(r.Direction());
        Float t = SphericalPhi(w) * Inv2Pi;
        Float s = SphericalTheta(w) * InvPi;
        return ibl(s, t);
      }
      return Color(0,0,0);
    }

    void PrintSettings();
    RenderSettings& Settings() { return _options;}
    void Settings(RenderSettings &opt) {
      _options = opt;
      }

    Image ibl;
    
  private:

    void _updateProgress();
    bool _getNextTile(int &tile);

    Camera _camera;
    shared_ptr<Primitive> _world;
    RenderSettings _options;
    Image _img;
    
    int _numTiles{0};
    int _numTilesWidth{0};

    int _renderedTiles{0};
    std::queue<int> _tilesToRender;

    std::vector<std::thread> _threads;
    std::mutex _mtx;
    std::mutex _mtx_cout;
};

// Generates the test scene
Scene GenerateTestScene(RenderSettings opt);

// Recursive raytracing function
Color Trace(const Ray& r, Scene *scene, int depth);

// Returns the Normal values
Color TraceNormalOnly(const Ray& r, Scene *scene);