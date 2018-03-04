#pragma once
#include "Vector.h"

namespace LtRenderer
{

class Ray
{
	Vec3 origin_;
	Vec3 direction_;
public:
	Ray() {};
	Ray(const Vec3& origin, const Vec3& direction): origin_(origin), direction_(direction){}

	Vec3 origin() const 
	{
		return origin_;
	}

	Vec3 direction() const
	{
		return direction_;
	}

	Vec3 pointAtParameter(double t) const
	{
		return origin_ + direction_ * t;
	}

};

}//namespace LtRenderer