#pragma once
#include "Vector.h"
#include "Constant.h"
#include "Material.h"


namespace LtRenderer
{
class Intersection
{
    Vec3 _normal;
    Vec3 _position;
    Material* _material;
public:
    double t;

    Intersection() : _normal(Vec3()), _position(Vec3())
    {
        t = D_HIT_MAX;
    };

    Vec3 normal() const
    {
        return _normal;
    }

    void setNormal(Vec3 normal)
    {
        _normal = normal;
    }

    Vec3 position() const
    {
        return _position;
    }

    void setPosition(Vec3 position)
    {
        _position = position;
    }

    Material* material() const
    {
        return _material;
    }

    void setMaterial(Material* material)
    {
        _material = material;
    }
};

}//namespace LtRenderer
