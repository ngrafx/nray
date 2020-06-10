# Nray

Nray is a multithreaded physically based raytracer made for the Capstone project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).
It is inspired from Peter Shirley's awsome Raytracing series (https://raytracing.github.io/) and the great PBRT book (https://www.pbrt.org/) written by Matt Pharr, Wenzel Jakob and Greg Humphreys.

## Building and Running Locally

### Basic Build Instructions
1. Make sure you have all the required dependencies
2. Clone this repo.
3. Make a build directory in the top level directory: `mkdir build && cd build`
4. Compile: `cmake .. && make`
5. Run it: `./nray`.

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

### Desciption and use

### Files and Classes Structure

## Additional Rubric Points

### Loops, Functions, I/O
The project demonstrates an understanding of C++ functions and control structures.

The project reads data from a file and process the data, or the program writes data to a file.
- Project loads a scene file and render an image on disk

The project accepts user input and processes the input.
- Project takes command line instructions from the user

### Object Oriented Programming
The project uses Object Oriented Programming techniques.

Classes use appropriate access specifiers for class members.

Class constructors utilize member initialization lists.
- All classes are using them when applicable (e.g. primitive.h)

Classes abstract implementation details from their interfaces.
- Have a look at the Image class (image.h)

Classes encapsulate behavior.

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
- Whenever the data is bigger than the simple data types I am using references. (e.g. l15 in image.h : SetPixel(int x, int y, Color &c))

The project uses destructors appropriately.
- As I'm only using smart pointers and not allocating anything 'manually' on the heap I didn't find the need to change the default Destructors.

The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate.
- This should be the case yes. I'm using make_shared for all the shared_ptr initialization

The project follows the Rule of 5.
- I'm following it although as I didn't need to 

The project uses move semantics to move data, instead of copying it, where possible.
- The Scene::Render methods return a std::move(Image) instead of copyting the data (scene.cpp l114)

The project uses smart pointers instead of raw pointers.
- I am only using Smart Pointers and am not using raw pointes

### Concurrency
The project uses multithreading.
- The Scene::Render() method uses multiple threads to render the image in tiles (scene.cpp l95)

A promise and future is used in the project.

A mutex or lock is used in the project.

A condition variable is used in the project.

