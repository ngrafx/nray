#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include "image.h"
#include "geometry.h"

Image::Image(int width, int height) : _width(width), _height(height), _channels(3) {
    _size = _width * _height * _channels;
    _pixels = make_unique<Float[]>(_size);
}


Color Image::operator()(int x, int y) const {
    int index = _Index(x, y);
    return Color(_pixels[index], _pixels[index+1], _pixels[index+2]);
}


void Image::SetPixel(int x, int y, const Color &c) {
    int index = _Index(x, y);
    _pixels[index] = c.x;
    _pixels[index+1] = c.y;
    _pixels[index+2] = c.z;
}

void Image::WriteToFile(char const *filename) const {
    unsigned char *img;
    img = new unsigned char[_width * _height * _channels];

    for (int i=0; i < _size; i++) {
        float val = _pixels[i];
        if (val != val)
            val = 0.0;

        // Gamma correction
        val = sqrt(val);

        img[i] = static_cast<unsigned char>(256 * Clamp(val, 0.0, 0.999));
    }

    stbi_write_png(filename, _width, _height, _channels, img, 0);
}

