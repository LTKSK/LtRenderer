#pragma once
#include <vector>
#include "Vector.h"


namespace LtRenderer
{

class Image
{
public:
	Image(int width, int height, std::vector<Vec3> pixels);
	~Image();
	Vec3 pixelFromUV(double u, double v);

private:
	int _width;
	int _height;
	std::vector<Vec3> _pixels;
};

class ImageLoader
{
public:
	ImageLoader();
	~ImageLoader();
	Image* load(std::string path);
};

}//namespace LtRenderer
