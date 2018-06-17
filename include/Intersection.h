#pragma once
#include "Vector.h"
#include "Constant.h"
#include "Material.h"


namespace LtRenderer
{
class Intersection
{
public:
    Intersection();
	~Intersection();
    double distance();
    void setDistance(double distance);
	Vec3 normal() const;
	void setNormal(Vec3 normal);
	Vec3 position() const;
	void setPosition(Vec3 position);
	Material* material() const;
	void setMaterial(Material* material);
	Vec2 uv();
	void setUv(Vec2 uv);

private:
    double _distance;
	Vec3 _normal;
	Vec3 _position;
	Vec2 _uv;
	Material* _material;
};

}//namespace LtRenderer
