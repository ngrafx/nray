#include "nray.h"

#include "rand.h"
#include "image.h"
#include "scene.h"
#include "timer.h"


// Initialize Random Number Generator
// We initialize the generator with a constant seed
// as we need the random number generation to be 
// deterministic
std::uniform_real_distribution<Float> Rng::distribution01 = std::uniform_real_distribution<Float>(0,1);
std::mt19937 Rng::generator = std::mt19937(0);


int main() {

    // Get Render options
    Options opt;
    opt.image_width = 400;
    opt.image_height = 200;
    opt.image_aspect_ratio = Float(opt.image_width) / opt.image_height;

    opt.tile_size = 16;

    opt.pixel_samples = 10;
    opt.max_ray_depth = 15;
    opt.image_out = "D:\\dev\\nray\\render3.png";

    std::cout << "___\n";
    std::cout << "Render Settings: \n";
    std::cout << "Image: " << opt.image_width << "x" << opt.image_height << "\n";
    std::cout << "Pixel samples: " << opt.pixel_samples << "\n";
    std::cout << "Ray Depth: " << opt.max_ray_depth << "\n\n";

    // Initialize timer
    Timer timer;
    
    // Generate Scene
    timer.Start();
    Scene scene(GenerateTestScene(opt));
    timer.Stop();
    std::cout << "Scene generated in: ";
    timer.Print();
    std::cout << "\n\n";

    // Create image buffer
    Image img;

    // Render the scene to the image
    timer.Start();
    img = scene.Render();
    timer.Stop();
    std::cout << "\nScene rendered in: ";
    timer.Print();
    std::cout << "\n\n";

    // Write the image to disk
    img.WriteToFile(opt.image_out);

    std::cerr << "Rendered image to " << opt.image_out << "\n";
}