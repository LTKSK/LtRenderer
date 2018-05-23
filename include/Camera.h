#pragma once
#include "Ray.h"

namespace LtRenderer
{

class Camera
{
    Vec3  _position;
    Vec3  _x_axis;
    Vec3  _y_axis;
    Vec3  _z_axis;
    double _near_clip;

public:
    Camera(Vec3 position, Vec3 direction, Vec3 up, double aspect_ratio, double near_clip)
        :_position(position), _near_clip(near_clip)
    {
        _x_axis    = normalize(cross(direction, up)) * aspect_ratio;
        _y_axis    = normalize(cross(_x_axis, direction));
        _z_axis    = direction;
    }

    Ray emit(double u, double v)
    {
        Vec3 ray_direction = _x_axis * u + _y_axis * v + _z_axis;
        Vec3 ray_position = _position + ray_direction * _near_clip;
        return Ray(ray_position, normalize(ray_direction));
    }
};

}//namespace LtRenderer
