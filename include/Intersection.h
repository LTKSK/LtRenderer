#pragma once
#include "Vector.h"
#include "Constant.h"
#include "Material.h"


namespace LtRenderer
{

//Rendering�ɕK�v�ȏՓˎ��̏����i�[����N���X
class Intersection
{
	Vec3 normal_;
	Vec3 position_;
	Material* material_;
public:
	float t;

	Intersection() : normal_(Vec3()), position_(Vec3())
	{
		t = F_HIT_MAX;
	};

	Vec3 normal() const
	{
		return normal_;
	}

	void setNormal(Vec3 normal)
	{
		normal_ = normal;
	}

	Vec3 position() const
	{
		return position_;
	}

	void setPosition(Vec3 position)
	{
		position_ = position;
	}

	Material* material() const
	{
		return material_;
	}

	void setMaterial(Material* material)
	{
		material_ = material;
	}
};

}//namespace LtRenderer
