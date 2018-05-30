#include "Intersection.h"


namespace LtRenderer
{

Intersection::Intersection() : _normal(Vec3()), _position(Vec3())
{
	t = D_HIT_MAX;
}

Intersection::~Intersection() {}

Vec3 Intersection::normal() const
{
	return _normal;
}

void Intersection::setNormal(Vec3 normal)
{
	_normal = normal;
}

Vec3 Intersection::position() const
{
	return _position;
}

void Intersection::setPosition(Vec3 position)
{
	_position = position;
}

Material* Intersection::material() const
{
	return _material;
}

void Intersection::setMaterial(Material* material)
{
	_material = material;
}

}//namespace LtRenderer