#pragma once
#include <vector>

namespace LtRenderer
{ 

class AABB
{
	Vec3 max_pos_;
	Vec3 min_pos_;

public:
	AABB() 
	{
		max_pos_ = Vec3(-F_MAX);
		min_pos_ = Vec3(F_MAX);
	}
	AABB(const Vec3 &max_pos, const Vec3 &min_pos): max_pos_(max_pos), min_pos_(min_pos){}
	~AABB() {}

	inline void merge(const AABB* aabb)
	{
		max_pos_ = Vec3(std::fmax(aabb->max_pos_.x(), max_pos_.x()),
						std::fmax(aabb->max_pos_.y(), max_pos_.y()),
						std::fmax(aabb->max_pos_.z(), max_pos_.z()));

		min_pos_ = Vec3(std::fmin(aabb->min_pos_.x(), min_pos_.x()),
						std::fmin(aabb->min_pos_.y(), min_pos_.y()),
						std::fmin(aabb->min_pos_.z(), min_pos_.z()));
	}

	inline bool contains(Ray& ray) const
	{
		return (ray.direction().x() >= min_pos_.x() && ray.direction().x() <= max_pos_.x() &&
				ray.direction().y() >= min_pos_.y() && ray.direction().y() <= max_pos_.y() &&
				ray.direction().z() >= min_pos_.z() && ray.direction().z() <= max_pos_.z());
	}

	Vec3 center() const
	{
		return (min_pos_ + max_pos_) * 0.5;
	}

	Vec3 size() const
	{
		return max_pos_ - min_pos_;
	}

	float surfaceArea() const
	{
		Vec3 size_vec = size();
		return (size_vec.x() * size_vec.y() + size_vec.x() * size_vec.z() + size_vec.y() * size_vec.z()) * 2.0;
	}

	inline bool intersect(const Ray& ray) 
	{
		//c‰¡‚ÅŒð·‚·‚ét‚Ì”ÍˆÍ‚ð’²‚×Atymax>txmin‚©‚ð”»’è‚·‚éB
		const Vec3 pos = ray.origin();
		const Vec3 dir = ray.direction();

		float t_max =  F_MAX;
		float t_min = -F_MAX;
		float t_near, t_far;
		
		float one_devide_dir = 1.0 / dir.x();
		float t1 = (max_pos_.x() - pos.x()) * one_devide_dir;
		float t2 = (min_pos_.x() - pos.x()) * one_devide_dir;
		t_near = std::fmin(t1, t2);
		t_far  = std::fmax(t1, t2);
		t_max  = std::fmin(t_max, t_far);
		t_min  = std::fmax(t_min, t_near);
		if (t_min > t_max)
		{
			return false;
		}

		one_devide_dir = 1.0 / dir.y();
		t1 = (max_pos_.y() - pos.y()) * one_devide_dir;
		t2 = (min_pos_.y() - pos.y()) * one_devide_dir;
		t_near = std::fmin(t1, t2);
		t_far  = std::fmax(t1, t2);
		t_max  = std::fmin(t_max, t_far);
		t_min  = std::fmax(t_min, t_near);
		if (t_min > t_max) 
		{
			return false;
		}

		one_devide_dir = 1.0 / dir.z();
		t1 = (max_pos_.z() - pos.z()) * one_devide_dir;
		t2 = (min_pos_.z() - pos.z()) * one_devide_dir;
		t_near = std::fmin(t1, t2);
		t_far  = std::fmax(t1, t2);
		t_max  = std::fmin(t_max, t_far);
		t_min  = std::fmax(t_min, t_near);
		if (t_min > t_max) 
		{
			return false;
		}
		return true;
	}
};

}//namespace LtRenderer
