#pragma once
#include "Vector.h"

namespace LtRenderer
{

class Ray
{
    Vec3 _origin;
    Vec3 _direction;
public:
    Ray() {};
    Ray(const Vec3& origin, const Vec3& direction): _origin(origin), _direction(direction){}

    Vec3 origin() const
    {
        return _origin;
    }

    Vec3 direction() const
    {
        return _direction;
    }

    Vec3 pointAtParameter(double t) const
    {
        return _origin + _direction * t;
    }

};

}//namespace LtRenderer