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


int main(int argc, char *argv[]) {

    std::cout << "\n\n";
    std::cout << "::::    ::: :::::::::      :::   :::   ::: \n";
    std::cout << ":+:+:   :+: :+:    :+:   :+: :+: :+:   :+: \n";
    std::cout << ":+:+:+  +:+ +:+    +:+  +:+   +:+ +:+ +:+  \n";
    std::cout << "+#+ +:+ +#+ +#++:++#:  +#++:++#++: +#++:   \n";
    std::cout << "+#+  +#+#+# +#+    +#+ +#+     +#+  +#+    \n";
    std::cout << "#+#   #+#+# #+#    #+# #+#     #+#  #+#    \n";
    std::cout << "###    #### ###    ### ###     ###  ###    \n\n";
    std::cout << "___\n\n";

    // Initialize timer
    Timer timer;
    
    // Generate Scene
    Scene scene;
    timer.Start();
    if (argc < 2) {
        std::cout << "\nNo scene specified, running test scene\n";
        RenderSettings opt;
        scene = GenerateTestScene(opt);
    }
    else {
        scene = LoadSceneFile(argv[1]);
    }
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
}