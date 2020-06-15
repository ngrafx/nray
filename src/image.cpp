#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include "image.h"
#include "geometry.h"

Image::Image(int width, int height) : _width(width), _height(height), _channels(3) {
    _size = _width * _height * _channels;
    _pixels = make_unique<Float[]>(_size);
}


Image::Image(const Image& other)
{
    // std::cout << "Image Copy Constructor" << std::endl;

    _width = other._width;
    _height = other._height;
    _channels = other._channels;
    _size = other._size;

    _pixels = make_unique<Float[]>(_size);
    std::copy(other._pixels.get(), other._pixels.get()+_size, _pixels.get());

}

Image::Image(Image&& other)
{
    // std::cout << "Image Move Constructor" << std::endl;

    _width = other._width;
    _height = other._height;
    _channels = other._channels;
    _size = other._size;

    _pixels = std::move(other._pixels);
}

Image& Image::operator=(const Image& other)
{
    // std::cout << "Image Copy Assignment Operator" << std::endl;

    if (&other != this) {
        _width = other._width;
        _height = other._height;
        _channels = other._channels;
        _size = other._size;

        _pixels = make_unique<Float[]>(_size);
        std::copy(other._pixels.get(), other._pixels.get()+_size, _pixels.get());
    }
    return *this;
}

Image& Image::operator=(Image&& other)
{
    // std::cout << "Image Move Assignment Operator" << std::endl;

    if (&other != this) {
        _width = other._width;
        _height = other._height;
        _channels = other._channels;
        _size = other._size;

        _pixels = std::move(other._pixels);
    }
    return *this;
}


Color Image::operator()(int x, int y) const {
    int index;
    if (!_Index(x, y, index))
        return Color();
    return Color(_pixels[index], _pixels[index+1], _pixels[index+2]);
}


void Image::SetPixel(int x, int y, const Color &c) {
    int index;
    if (!_Index(x, y, index))
        return;
    _pixels[index] = c.x;
    _pixels[index+1] = c.y;
    _pixels[index+2] = c.z;
}

bool Image::_Index(int x, int y, int &index) const {
    index = -1;
    if ( (x < 0) || (x >= _width) || (y < 0) || (y >= _height) )
        return false;
    index =  (x + y * _width) * _channels;
    return true;
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

