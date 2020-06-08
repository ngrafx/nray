#pragma once

#include "nray.h"

class Image {
  public:
    Image(int width, int height);

    // Returns the Color at pixel (x, y);
    Color operator()(int x, int y) const;
    Color& operator()(int x, int y);

    // Sets the Color at pixel (x, y);
    void SetPixel(int x, int y, const Color &c);

    // Writes the Image as a *.png file
    void WriteToFile(char const *filename) const;

  private:
    const int _width;
    const int _height;
    const int _channels;

    unique_ptr<Float[]> _pixels;
    int _size;

    int _Index(int x, int y) const {return (x + y * _width) * _channels;}
};