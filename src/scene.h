#pragma once

#include <thread>
#include <mutex>
#include <queue>

#include "nray.h"
#include "image.h"
#include "camera.h"
#include "primitive.h"


// RenderSettings
struct RenderSettings {

  // Image 
  int image_width{200};
  int image_height{100};
  Float image_aspect_ratio{2};

  // Tile size (used to divide the image in tiles)
  int tile_size{16};

  // Number of samples per pixel
  int pixel_samples{20};

  // Maximum Ray Depth
  // TODO: split this between Diffuse, Reflect & Refract
  int max_diffuse_rdepth{2};
  int max_reflect_rdepth{5};
  int max_refract_rdepth{5};


  // When reached the max ray depth
  // returns bg (env) color instead of black
  bool useBgColorAtLimit{false};

  // Color limit
  // Clamps the color sample to this max value
  int color_limit{10};

  // Set the renderer in Normal Only mode
  // No lighting computation, just returns
  // the normals
  bool normalOnly{false};

  // Limit the number of threads (if >0)
  int max_threads{-1};
  
  // Output image path
  char const *image_out{"./out.png"};
};

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

    shared_ptr<Primitive> World() { return _world;}

    // Render the scene to an image
    Image Render();

    // Sample the environment color
    Color SampleEnvironment(const Ray &r) {
      if (ibl.Valid()) {
        Vec3 w = Normalize(r.Direction());
        Float t = SphericalPhi(w) * Inv2Pi;
        Float s = SphericalTheta(w) * InvPi;
        return ibl(s, t);
      }
      return Color(0,0,0);
    }

    // Print Render Settings
    void PrintSettings();
    // Returns Render Settings
    RenderSettings& Settings() { return _options;}
    // Sets render settings
    void Settings(RenderSettings &opt) {
      _options = opt;
      }

    Image ibl;
    
  private:

    // Update the render progress
    void _updateProgress();
    // Get next tile in the queue
    bool _getNextTile(int &tile);
    // Render the tiles
    void _RenderTile();

    Camera _camera;
    shared_ptr<Primitive> _world;
    RenderSettings _options;

    // Output Image
    Image _img;
    
    // Total Number of tiles
    int _numTiles{0};
    // Number of tiles along the img width
    int _numTilesWidth{0};

    // Number of tiles currently rendered
    int _renderedTiles{0};
    // Tiles left to Render
    std::queue<int> _tilesToRender;

    // Threads & locks
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