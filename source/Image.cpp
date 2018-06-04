#include <stb/stb_image.h>
#include "Image.h"

namespace LtRenderer
{
ImageLoader::ImageLoader() {};
ImageLoader::~ImageLoader() {};

Image* ImageLoader::load(std::string path) 
{
	int bit_per_pixel;
	int width, height;
	auto bg_image = stbi_load(path.c_str(), &width, &height, &bit_per_pixel, 0);
	std::vector<Vec3> pixels;
	pixels.resize(width * height);
	for (int i = 0; i < width * height; ++i)
	{
		pixels[i] = Vec3(bg_image[i * 3 + 0] / 255.0,
						 bg_image[i * 3 + 1] / 255.0,
					     bg_image[i * 3 + 2] / 255.0);
	}
	return new Image(width, height, pixels);
}


Image::Image(int width, int height, std::vector<Vec3> pixels): _width(width), 
															   _height(height),
															   _pixels(pixels)
{}

Image::~Image()
{
	_pixels.clear();
}

Vec3 Image::pixelFromUV(double u, double v)
{
	int x = int(u * _width);
	int y = int(v * _height);
	return _pixels[x + y * _width];
}

}//namespace LtRenderer