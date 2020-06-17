#pragma once

#include "nray.h"

class Image {
  public:
    Image() {}
    Image(int width, int height);

    Image(const Image& other); // copy constructor
    Image(Image&& other); // move constructor
    Image& operator=(const Image& other); // copy assignment operator
    Image& operator=(Image&& other); // move assignment operator
    ~Image() {}

    // Returns the Color at pixel (x, y)
    // x = [0, _width)  y = [0, _height)
    Color operator()(int x, int y) const;

    // Returns the Color at pixel (s, t)
    // s = [0,1]  t = [0,1]
    Color operator()(Float s, Float t) const;

    // Sets the Color at pixel (x, y);
    void SetPixel(int x, int y, const Color &c);

    // Loads an Image from a file
    void LoadFromFile(char const *filename);

    // Writes the Image as a *.png file
    void WriteToFile(char const *filename) const;

    int Width() {return _width;}
    int Height() {return _height;}

    bool Valid() { 
      return (_size > 0);
    }

  private:
    int _width{0};
    int _height{0};
    int _channels{3};

    unique_ptr<Float[]> _pixels;
    int _size{0};

    bool _Index(int x, int y, int &index) const;
};