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
    opt.image_width = 200;
    opt.image_height = 100;
    opt.image_aspect_ratio = Float(opt.image_width) / opt.image_height;

    opt.pixel_samples = 10;
    opt.max_ray_depth = 15;
    opt.image_out = "D:\\dev\\nray\\render3.png";

    // Initialize timer
    Timer timer;
    
    // Generate Scene
    timer.Start();
    Scene scene(GenerateTestScene(opt));
    timer.Stop();
    std::cout << "Scene generated in: ";
    timer.Print();
    std::cout << std::endl;

    // Create image buffer
    Image img(opt.image_width, opt.image_height);

    // Render the scene to the image
    timer.Start();
    scene.Render(img);
    timer.Stop();
    std::cout << "\nScene rendered in: ";
    timer.Print();
    std::cout << std::endl;

    // Write the image to disk
    img.WriteToFile(opt.image_out);

    std::cerr << "Rendered image to " << opt.image_out << "\n";
}