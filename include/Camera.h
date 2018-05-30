#pragma once
#include "Ray.h"

namespace LtRenderer
{

class Camera
{
public:
	Camera(Vec3 position, Vec3 direction, Vec3 up, double aspect_ratio, double near_clip);
	~Camera();
	Ray emit(double u, double v);

private:
	Vec3  _position;
	Vec3  _x_axis;
	Vec3  _y_axis;
	Vec3  _z_axis;
	double _near_clip;
};

}//namespace LtRenderer
