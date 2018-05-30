#pragma once
#include "Vector.h"
#include "Constant.h"
#include "Material.h"


namespace LtRenderer
{
class Intersection
{
public:
	double t;
    Intersection();
	~Intersection();
	Vec3 normal() const;
	void setNormal(Vec3 normal);
	Vec3 position() const;
	void setPosition(Vec3 position);
	Material* material() const;
	void setMaterial(Material* material);

private:
	Vec3 _normal;
	Vec3 _position;
	Material* _material;
};

}//namespace LtRenderer
