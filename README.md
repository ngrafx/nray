# Nray
![Nray][img5]
## Project Overview

Nray is a multithreaded physically based path tracer. It reads a scene file that describes a camera, a lighting environment and several objects. It then renders the scene using the raytracing (and raymarching) algorithm and outputs an image. It is written in modern c++, and apart from the header only [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) and [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h) it is not using any other libraries so it's easy to compile and run on different architectures. It is a learning project so I tried implementing myself every feature.

Raytracing & Raymarching are very well documented, here are some of the resources I used :
- [Peter Shirley's Raytracing series](https://raytracing.github.io/)
- [Matt Pharr, Wenzel Jakob and Greg Humphreys PBRT book](https://www.pbrt.org/)
- [Scratch a Pixel](https://www.scratchapixel.com/)

Nray comes with a few sample scenes and objects as well as some HDR images to use for Image-Based-Lighting. It can currently handle the following primitives :
- Sphere
- Triangle Meshes, reads as [.obj files](https://en.wikipedia.org/wiki/Wavefront_.obj_file)
- Implicit Surfaces (SDF)

It has a few different Materials defining how the objects interacts with lights :
- Lambertian
- Dielectric
- Metal
- Emissive

Here's a few example renders :

`./nray ../scenes/broken_bunny.nray`
![Broken Bunny][img3]
Triangle Meshes only, 700 samples, 1024x576, 25 max bounces, 4hr30min on 40 threads (Intel(R) Xeon(R) CPU E5-2630 v4 @ 2.20GHz)


`./nray ../scenes/cornell_box.nray`
![Cornell Box][img1]
Triangle Meshes & Sphere primitives, 1500 samples, 1024x576, 15 max bounces, 1hr5min on 12 threads (Intel(R) Core(TM) i7-9750H CPU @ 2.60Ghz)


`./nray --testScene`
![Test Scene][img2]
Sphere primitives and Implicit (raymarched) Boxes, 500 samples, 1024x576, 5 max bounces (Diffuse, Reflect & Refract), 33min on 12 threads (Intel(R) Core(TM) i7-9750H CPU @ 2.60Ghz)


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

Options:

 -o /path/to/output/image.png
        Output the image to this path

 -iw width
        Sets the pixel width

 -ih height
        Sets the pixel height

 -s number_of_pixel_samples
        Sets the pixel samples

 -depth_refl ray_depth
        Sets the maximum depth/bounces for Reflection Rays, defaults to 5

 -depth_refr ray_depth
        Sets the maximum depth/bounces for Refraction Rays, defaults to 5

 -depth_diff ray_depth
        Sets the maximum depth/bounces for Diffuse Rays, defaults to 2

 -t tile_size
        Sets the tile size (defaults to 16)

 -j max_threads
        Limits the max number of threads

 -color_limit max_value
        Clamps the maximum color value. Can help reduce fireflies with very bright lights (defaults to 10)

 --useBgColorAtLimit
        Use the background (environment) color at ray limit instead of black

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

[img1]:                   images/cornell_box.png
[img2]:                   images/test_scene.png
[img3]:                   images/broken_bunny.png
[img4]:                   images/broken_bunny_normals.png
[img5]:                   images/vignette.jpg