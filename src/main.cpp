#include <string.h>
#include "nray.h"

#include "rand.h"
#include "image.h"
#include "scene.h"
#include "timer.h"

#include "parser.h"


// Initialize Random Number Generator
// We initialize the generator with a constant seed
// as we need the random number generation to be 
// deterministic
std::uniform_real_distribution<Float> Rng::distribution01 = std::uniform_real_distribution<Float>(0,1);
std::mt19937 Rng::generator = std::mt19937(0);

void PrintUsage() {
    std::cout << "\nUsage:\n";

    std::cout << "\n nray /path/to/scene.nray\n";
    std::cout << "\tRenders the specified scene\n";

    std::cout << "\n nray --testScene\n";
    std::cout << "\tRenders the test scene\n";

    std::cout << "\nOptional: \n";

    std::cout << "\n -o /path/to/output/image.png\n";
    std::cout << "\tOutput the image to this path\n";

    std::cout << "\n -iw width\n";
    std::cout << "\tSets the image's pixel width\n";

    std::cout << "\n -ih height\n";
    std::cout << "\tSets the image's pixel height\n";

    std::cout << "\n -s number_of_pixel_samples\n";
    std::cout << "\tSets the pixel samples\n";

    std::cout << "\n -d ray_depth\n";
    std::cout << "\tSets the maximum ray depth (number of light bounces)\n";

    std::cout << "\n -t tile_size\n";
    std::cout << "\tSets the tile size (defaults to 16)\n";

    std::cout << "\n -j max_threads\n";
    std::cout << "\tLimits the max number of threads\n";

    std::cout << "\n --normalOnly\n";
    std::cout << "\tRender the Scene's normal only. No Lighting/material computation\n";
}


int main(int argc, char *argv[]) {

    // If not enough arguments specified
    if (argc < 2) {
        PrintUsage();
        return -1;
    }

    // Initialize timer
    Timer timer;
    
    // Parse arguments before scene generation
    bool test_scene = false;
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "--testScene") == 0) {
            test_scene = true;
        }
        else if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 ) {
            PrintUsage();
            return 0;
        }
    }

    std::cout << "\n\n";
    std::cout << "::::    ::: :::::::::      :::   :::   ::: \n";
    std::cout << ":+:+:   :+: :+:    :+:   :+: :+: :+:   :+: \n";
    std::cout << ":+:+:+  +:+ +:+    +:+  +:+   +:+ +:+ +:+  \n";
    std::cout << "+#+ +:+ +#+ +#++:++#:  +#++:++#++: +#++:   \n";
    std::cout << "+#+  +#+#+# +#+    +#+ +#+     +#+  +#+    \n";
    std::cout << "#+#   #+#+# #+#    #+# #+#     #+#  #+#    \n";
    std::cout << "###    #### ###    ### ###     ###  ###    \n\n";
    std::cout << "___\n\n";

    // Generate Scene
    Scene scene;
    timer.Start();
    if (test_scene) {
        std::cout << "\nRendering Test Scene\n";
        RenderSettings opt;
        scene = GenerateTestScene(opt);
    }
    else {
        std::cout << "\nRendering " << argv[1] << "\n";
        scene = LoadSceneFile(argv[1]);
    }

    // Parse the arguments again for scene settings override
    RenderSettings opt = scene.Settings();
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            opt.image_out = argv[i+1];
        }
        else if(strcmp(argv[i], "-iw") == 0) {
            opt.image_width = std::stoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "-ih") == 0) {
            opt.image_height = std::stoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "-s") == 0) {
            opt.pixel_samples = std::stoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "-d") == 0) {
            opt.max_ray_depth = std::stoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "-t") == 0) {
            opt.tile_size = std::stoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--normalOnly") == 0) {
            opt.normalOnly = true;
        }
        else if (strcmp(argv[i], "-j") == 0) {
            opt.max_threads = std::stoi(argv[i+1]);
        }
    }

    scene.Settings(opt);
    scene.PrintSettings();
    timer.Stop();
    std::cout << "\nScene generated in: ";
    timer.Print();
    std::cout << "\n";

    // Create image buffer
    Image img;

    // Render the scene to the image
    timer.Start();
    img = scene.Render();
    timer.Stop();
    std::cout << "\nScene rendered in: ";
    timer.Print();

    // Write the image to disk
    img.WriteToFile(scene.Settings().image_out);

    std::cerr << "\nRendered image to " << scene.Settings().image_out << "\n";
    return 0;
}