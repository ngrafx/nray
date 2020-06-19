# Nray
![Nray][img4]
## Project Overview

Nray is a multithreaded physically based raytracer. It reads a scene file that describes a camera, a lighting environment and several objects. It then renders the scene using the raytracing algorithm and outputs an image. It is written in modern c++, and apart from the header only [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) and [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h) it is not using any other libraries so it's easy to compile and run on different architectures. It is a learning project so I tried implementing myself every feature.

Raytracing is very well documented, here are some of the resources I used :
- [Peter Shirley's Raytracing series](https://raytracing.github.io/)
- [Matt Pharr, Wenzel Jakob and Greg Humphreys PhysicallyBasedRayTracing](https://www.pbrt.org/)
- [Scratch a Pixel](https://www.scratchapixel.com/)

Nray comes with a few sample scenes and objects as well as some HDR images to use for Image-Based-Lighting. It can currently handle Spheres and Triangle Meshes and reads [.obj files](https://en.wikipedia.org/wiki/Wavefront_.obj_file)
It has a few different Materials defining how the objects interacts with the lights :
- Lambertian
- Dielectric
- Metal
- Emissive

Here's a few example renders :

![Broken Bunny][img3]
700 samples, 1024x576, 25 max bounces, 4hr30min on 40 threads (Intel(R) Xeon(R) CPU E5-2630 v4 @ 2.20GHz)
`./nray ../scenes/broken_bunny.nray`

![Cornell Box][img1]
1500 samples, 1024x576, 15 max bounces, 1hr5min on 12 threads (Intel(R) Core(TM) i7-9750H CPU @ 2.60Ghz)
`./nray ../scenes/cornell_box.nray`

![Test Scene][img2]
1500 samples, 1024x576, 15 max bounces, 9min50 on 12 threads (Intel(R) Core(TM) i7-9750H CPU @ 2.60Ghz)
`./nray --testScene`

## Building and Running Locally

### Basic Build Instructions
1. Make sure you have all the required dependencies
2. Clone this repo.
3. Make a build directory in the top level directory: `mkdir build && cd build`
4. Compile: `cmake .. -DCMAKE_BUILD_TYPE=Release && make`
5. Run it: `./nray --testScene`

### Dependencies for Running Locally
* cmake >= 3.6
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)


## Description and use

nray is a command line tool that takes a scene as an input and renders it as an image.

It comes with 3 samples scenes that can be modified to render different things. Notes that the above example renders have been run with higher samples. I've kept the demo scenes relatively fast even on a modest computer.

`./nray ../scenes/cornell_box.nray`

It also takes a few command line arguments :
```
Usage:

 nray /path/to/scene.nray
        Renders the specified scene

 nray --testScene
        Renders the test scene

Optional:

 -o /path/to/output/image.png
        Output the image to this path

 -iw width
        Sets the pixel width

 -ih height
        Sets the pixel height

 -s number_of_pixel_samples
        Sets the pixel samples

 -d ray_depth
        Sets the maximum ray depth (number of light bounces)

 -t tile_size
        Sets the tile size (defaults to 16)

 -j max_threads
        Limits the max number of threads

 --normalOnly
        Render the Scene's normal only. No Lighting/material computation

```

The --normalOnly mode is very useful for debugging as it bypass all the lighting & material computation as well as all the secondary rays and just outputs the Normal values as a Color. It is then much faster to render.
Note that to avoid being clamped by the image format normals are remapped from  (-1,1) to (0,1).

![Broken Bunny Normals][img4]

Scene files are basically text files that describe the different elements to load in the scene. Have a look at [scenes/scene_template.nray](scenes/scene_template.nray) and the sample scenes for an detailed example of what is available.


### Files and Classes Structure

Once the program runs, a [Scene](src/scene.h) is created and populated with multiple [Primitives](src/primitive.h). Primitive represent objects that can be intersected and thus traced recursively by the main rendering function. Primitive have [Materials](src/material.h) that describe how the light interacts with them. The Scene::Render method splits the image to render into multiple small sections (called tiles) and add them to a queue. The method then initializes multiple threads and have them render all the tiles, one after the other.

For each tile we use the [Camera](src/camera.h) to throw multiple [Rays](src/geometry.h) (one per pixel samples to be correct) through every pixel. We then find out if the Ray intersect any scene Primitive. If so then we compute the lighting information using its Material and scatter the Ray further. We then take the average of all those color samples and set the final pixel color in the [Image](src/image.h). Once all the thread have finished rendering all the tiles we write the Image to disk as a .png file and exit the program.

The rest of the code just helps support and implement those main classes, for example the [parsing functions](src/parser.h) parse the different file inputs to generate data that nray understands.

All the source files are in [src/](src/)

External header libraries are in [external/](external/)

Sample scenes in [senes/](scenes/) as well as the [objs](scenes/objs/) and [hdr images](scenes/maps/) they reference

## Additional Rubric Points

### Loops, Functions, I/O
The project demonstrates an understanding of C++ functions and control structures.
- I hope so!

The project reads data from a file and process the data, or the program writes data to a file.
- Project loads a scene file and render an image on disk

The project accepts user input and processes the input.
- Project takes a scene file from the user and additional command line instructions

### Object Oriented Programming
The project uses Object Oriented Programming techniques.
- yes

Classes use appropriate access specifiers for class members.
- Yes if the class members are invariant.

Class constructors utilize member initialization lists.
- All classes are using them when applicable (e.g. primitive.h)

Classes abstract implementation details from their interfaces.
- Have a look at the [Image class](src/image.h). For example we can query and set the pixel [Colors](src/nray.h) by specifying x & y coordinates although the data is actually stored as a Float array. This simplify the use of the class (as the rest of the programs 'thinks' in 2d coordinates) regardless of how the class stores data internally

Classes encapsulate behavior.
- In the [Image class](src/image.h) all the image related operations are encapsulated

Classes follow an appropriate inheritance hierarchy.
- In [primitive.h](src/primitive.h) you can see that all the primitives inherit from the pure virtual Primitive() class, same goes with all the Materials

Overloaded functions allow the same function to operate on different parameters.
- In [geometry.h](src/geometry.h) you can see that we're overloading all the operator for the templated Vector3<> class. Classes constructors are often overloaded and for example the Image::operator() is overloaded so it can performs different actions if it's given integers (pixel coords) or floats (normalized pixel coordinates)

Derived class functions override virtual base class functions.
- You can see all the children Primitive class in [primitive.h](src/primitive.h)

Templates generalize functions in the project.
- The Vector3 class is a templated class, implemented in [geometry.h](src/geometry.h). There's also some templated functions in nray.h

### Memory Management
The project makes use of references in function declarations.
- Whenever the data is bigger than the simple data types I am using references. (e.g. l30 in [image.h](src/image.h) : SetPixel(int x, int y, Color &c))

The project uses destructors appropriately.
- As I'm only using smart pointers and not allocating anything 'manually' on the heap I didn't find the need to change the default Destructors.

The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate.
- This should mostly be the case. For example I'm using make_shared and make_unique for all the smart pointers intializations

The project follows the Rule of 5.
- Project does follow the rule of 5. Both [Image](src/image.h) & [Scene](src/scene.h) have all 5 implemented as I needed to control their move mechanism

The project uses move semantics to move data, instead of copying it, where possible.
- The Scene::Render methods return a std::move(Image) instead of copyting the data ([scene.cpp](src/scene.cpp) l158). Same technique is used throughout the project, in  CreateTriangleMesh() for example [primitive.cpp](src/primitive.cpp), l258

The project uses smart pointers instead of raw pointers.
- I am only using Smart Pointers and am not directly using raw pointers apart as arguments in certain function calls as per the Core C++ guidelines

### Concurrency
The project uses multithreading.
- The Scene::Render() method uses multiple std::threads to render the image in tiles ([scene.cpp](src/scene.cpp) l123)

A promise and future is used in the project.
- Didn't really need to use this for now

A mutex or lock is used in the project.
- The Scene::_updateProgress() and  Scene::_getNextTile() uses a [lock (and a mutex)](src/scene.cpp) l72 to prevent data races amongst threads.

A condition variable is used in the project.
- Didn't really need to use this for now

[img1]:                   images/cornell_box.png
[img2]:                   images/test_scene.png
[img3]:                   images/broken_bunny.png
[img4]:                   images/broken_bunny_normals.png
[img5]:                   images/vignette.jpg