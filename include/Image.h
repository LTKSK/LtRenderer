#pragma once
#include <vector>
#include "Vector.h"

namespace LtRenderer
{

class Image
{
public:
	Image(int width, int height);
	Image(std::string path);
	~Image();
	Vec3 pixelFromUV(double u, double v);

private:
	int _width;
	int _height;
	std::vector<Vec3> pixels;
};

}//namespace LtRenderer
