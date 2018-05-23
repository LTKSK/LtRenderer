#pragma once
#include <vector>
#include <stb/stb_image.h>
#include "Vector.h"

namespace LtRenderer
{

class Image
{
    int _width;
    int _height;
    std::vector<Vec3> pixels;
public:
    Image(int width, int height) : _width(width), _height(height)
    {
        pixels.resize(width * height * 3);
    }

    Image(std::string path)
    {
        int bit_per_pixel;
        auto bg_image = stbi_load(path.c_str(), &_width, &_height, &bit_per_pixel, 0);
        pixels.resize(_width * _height);
        for (int i = 0; i < _width * _height; ++i)
        {
            pixels[i] = Vec3(bg_image[i * 3 + 0] / 255.0,
                             bg_image[i * 3 + 1] / 255.0,
                             bg_image[i * 3 + 2] / 255.0);
        }
    }

    ~Image()
    {
        pixels.clear();
    }

    Vec3 pixelFromUV(double u, double v)
    {
        int x = int(u * _width);
        int y = int(v * _height);
        return pixels[x + y * _width];
    }

};

}//namespace LtRenderer
