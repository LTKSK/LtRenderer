#pragma once
#include <vector>
#include "Vector.h"
#include "Constant.h"
#include "Ray.h"

namespace LtRenderer
{

class AABB
{
public:
	AABB();
	AABB(const Vec3 &max_pos, const Vec3 &min_pos);
	~AABB();

	void merge(const AABB* aabb);
	bool contains(Ray& ray) const;
	Vec3 center() const;
	Vec3 size() const;
	double surfaceArea() const;
	bool intersect(const Ray& ray);

private:
	Vec3 _max_pos;
	Vec3 _min_pos;
};

}//namespace LtRenderer
