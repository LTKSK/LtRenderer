#pragma once
#include <vector>

namespace LtRenderer
{

class AABB
{
    Vec3 _max_pos;
    Vec3 _min_pos;

public:
    AABB()
    {
        _max_pos = Vec3(-D_MAX);
        _min_pos = Vec3(D_MAX);
    }
    AABB(const Vec3 &max_pos, const Vec3 &min_pos): _max_pos(max_pos), _min_pos(min_pos){}
    ~AABB() {}

    inline void merge(const AABB* aabb)
    {
        _max_pos = Vec3(std::fmax(aabb->_max_pos.x(), _max_pos.x()),
                        std::fmax(aabb->_max_pos.y(), _max_pos.y()),
                        std::fmax(aabb->_max_pos.z(), _max_pos.z()));

        _min_pos = Vec3(std::fmin(aabb->_min_pos.x(), _min_pos.x()),
                        std::fmin(aabb->_min_pos.y(), _min_pos.y()),
                        std::fmin(aabb->_min_pos.z(), _min_pos.z()));
    }

    inline bool contains(Ray& ray) const
    {
        return (ray.direction().x() >= _min_pos.x() && ray.direction().x() <= _max_pos.x() &&
                ray.direction().y() >= _min_pos.y() && ray.direction().y() <= _max_pos.y() &&
                ray.direction().z() >= _min_pos.z() && ray.direction().z() <= _max_pos.z());
    }

    Vec3 center() const
    {
        return (_min_pos + _max_pos) * 0.5;
    }

    Vec3 size() const
    {
        return _max_pos - _min_pos;
    }

    double surfaceArea() const
    {
        Vec3 size_vec = size();
        return (size_vec.x() * size_vec.y() + size_vec.x() * size_vec.z() + size_vec.y() * size_vec.z()) * 2.0;
    }

    inline bool intersect(const Ray& ray)
    {
        const Vec3 pos = ray.origin();
        const Vec3 dir = ray.direction();

        double t_max =  D_MAX;
        double t_min = -D_MAX;
        double t_near, t_far;

        double one_devide_dir = 1.0 / dir.x();
        double t1 = (_max_pos.x() - pos.x()) * one_devide_dir;
        double t2 = (_min_pos.x() - pos.x()) * one_devide_dir;
        t_near = std::fmin(t1, t2);
        t_far  = std::fmax(t1, t2);
        t_max  = std::fmin(t_max, t_far);
        t_min  = std::fmax(t_min, t_near);
        if (t_min > t_max)
        {
            return false;
        }

        one_devide_dir = 1.0 / dir.y();
        t1 = (_max_pos.y() - pos.y()) * one_devide_dir;
        t2 = (_min_pos.y() - pos.y()) * one_devide_dir;
        t_near = std::fmin(t1, t2);
        t_far  = std::fmax(t1, t2);
        t_max  = std::fmin(t_max, t_far);
        t_min  = std::fmax(t_min, t_near);
        if (t_min > t_max)
        {
            return false;
        }

        one_devide_dir = 1.0 / dir.z();
        t1 = (_max_pos.z() - pos.z()) * one_devide_dir;
        t2 = (_min_pos.z() - pos.z()) * one_devide_dir;
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
