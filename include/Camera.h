#pragma once
#include "Ray.h"

namespace LtRenderer
{

class Camera
{
	Vec3  position_;
	Vec3  x_axis_;
	Vec3  y_axis_;
	Vec3  z_axis_;
	float near_clip_;

public:
	Camera(Vec3 position, Vec3 direction, Vec3 up, float aspect_ratio, float near_clip)
		:position_(position), near_clip_(near_clip)
	{
		//スクリーンの縦横方向の基底ベクトル
		x_axis_    = normalize(cross(direction, up)) * aspect_ratio;
		y_axis_    = normalize(cross(x_axis_, direction));
		z_axis_    = direction;
	}

	Ray emit(float u, float v)
	{
		Vec3 ray_direction = x_axis_ * u + y_axis_ * v + z_axis_;
		Vec3 ray_position = position_ + ray_direction * near_clip_;
		return Ray(ray_position, normalize(ray_direction));
	}
};

}//namespace LtRenderer
