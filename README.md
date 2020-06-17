# Nray

[img1]

Nray is a multithreaded physically based raytracer made for the Capstone project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).
It is very much inspired from Peter Shirley's awsome Raytracing series (https://raytracing.github.io/) with some design ideas from the great PBRT book (https://www.pbrt.org/) written by Matt Pharr, Wenzel Jakob and Greg Humphreys.

## Building and Running Locally

### Basic Build Instructions
1. Make sure you have all the required dependencies
2. Clone this repo.
3. Make a build directory in the top level directory: `mkdir build && cd build`
4. Compile: `cmake .. -DCMAKE_BUILD_TYPE=Release && make`
5. Run it: `./nray`

### Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Project Overview

Nray is a multithreaded offline physically based raytracer. It reads a scene file that describes a camera, a lighting environment and several objects. It then use the raytracing algorithm to render it and output an image. Note that apart from stb_image.h I am not using any other library. This project is really a learning exercise and therefore I decided to implement every feature.
Most of the math and ideas behind the project can be found in Peter Shirley's awsome Raytracing series (https://raytracing.github.io/) as well in the great PBRT book (https://www.pbrt.org/) written by Matt Pharr, Wenzel Jakob and Greg Humphreys.

Once the program runs, a Scene is created and populated with multiple Primitive(s). Primitive represent objects that can be intersected and thus traced recursively by the main rendering function. Primitive have Material that describe how the light interacts with them. The Scene::Render method intialize multiple thread and have them render small sections of the Image (called tiles) at a time.
For each tile we throw Ray(s) through the Camera and find out if the Ray intersect any scene Primitive. If so then we compute the lighting information and scatter the Ray further.

### Description and use

Nray comes with 3 samples scenes that can be modified to render different things. The renderer can handle Spheres and Triangle Meshes, the latter can be read as an .obj file
Once the project is built you can pass a scene file and run it
`./nray ../scenes/cornell_box.nray`

### Files and Classes Structure

Scene is the main class that gather a world (multiple Primitive) and render through a Camera to output an Image. Scene::Render() sends Ray to intersect with the world and return Color values;
Primitives are the intersectable objects. Primitive is the base virtual class and every other object is derived from there (BVH, Triangle...)
Material is what describe the Primitive surface's properties, how it interacts with the light. There's a few different Material, all inheriting from the base class (LambertianMaterial, DielectricMaterial...)

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

Classes use appropriate access specifiers for class members.

Class constructors utilize member initialization lists.
- All classes are using them when applicable (e.g. primitive.h)

Classes abstract implementation details from their interfaces.
- Have a look at the Image class (image.h). For example we can query and set the pixel Color by specifying x & y coordinates although the data is actually stored as a Float array. This simplify the use of the class regardless of how the class stores data internally

Classes encapsulate behavior.
- In the image class (image.h) all the image related operations are encapsulated

Classes follow an appropriate inheritance hierarchy.
- In primitive.h you can see that all the primitives inherit from the pure virtual Primitive() class

Overloaded functions allow the same function to operate on different parameters.
- In geometry.h you can see that we're overloading all the operator for the templated Vector3<> class

Derived class functions override virtual base class functions.
- You can see all the children Primitive class in primitive.h

Templates generalize functions in the project.
- The Vector3 class is a templated class, implemented in geometry.h. There's also some templated functions in nray.h

### Memory Management
The project makes use of references in function declarations.
- Whenever the data is bigger than the simple data types I am using references. (e.g. l24 in image.h : SetPixel(int x, int y, Color &c))

The project uses destructors appropriately.
- As I'm only using smart pointers and not allocating anything 'manually' on the heap I didn't find the need to change the default Destructors.

The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate.
- This should mostly be the case. For example I'm using make_shared and make_unique for all the smart pointers intializations

The project follows the Rule of 5.
- Project does follow the rule of 5. Both Image (image.h) & Scene (scene.h) have all 5 implemented as I needed to control their move mechanism

The project uses move semantics to move data, instead of copying it, where possible.
- The Scene::Render methods return a std::move(Image) instead of copyting the data (scene.cpp l152). Same technique is used throughout the project, in  CreateTriangleMesh() for example (primitive.cpp, l258)

The project uses smart pointers instead of raw pointers.
- I am only using Smart Pointers and am not directly using raw pointers apart as arguments in certain function calls as per the Core C++ guidelines

### Concurrency
The project uses multithreading.
- The Scene::Render() method uses multiple std::threads to render the image in tiles (scene.cpp l120)

A promise and future is used in the project.

A mutex or lock is used in the project.
- The Scene::_updateProgress() and  Scene::_getNextTile() uses a lock (and a mutex) to prevent data races amongst threads.

A condition variable is used in the project.

[img1]:                   images/cornell_box.png