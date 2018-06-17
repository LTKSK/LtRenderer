#pragma once
#include "Constant.h"
#include "Ray.h"
#include "Material.h"
#include "Intersection.h"
#include "Collision.h"
#include "Vertex.h"

namespace LtRenderer
{

class Mesh
{

public:
	Mesh();
	~Mesh();
    virtual inline Vec3 position() const = 0;
    virtual inline bool intersect(const Ray& ray, Intersection *intersection) const = 0;
    virtual inline Material* material() const = 0;
    virtual inline AABB* boundingBox() const = 0;
    virtual inline Vec3 randomPoint(Random* random) const = 0;
    virtual double surfaceArea() const = 0;
};

class Sphere : public Mesh
{
public:
	Sphere(Vec3 center, double radius, Material* material);
	~Sphere();

	Material* material() const;
	AABB* boundingBox() const;
	Vec3 position() const;
	double surfaceArea() const;
	Vec3 randomPoint(Random* random) const;
	bool intersect(const Ray& ray, Intersection *intersection) const;

private:
	Vec3 _position;
	double _radius;
	double _surface_area;
	Material* _material;
	AABB* _bounding_box;
};

class Triangle : public Mesh
{
public:
    Triangle(const Vertex& vertex_a,
             const Vertex& vertex_b,
             const Vertex& vertex_c,
             Material* material);
    ~Triangle();
	Material* material() const;
	AABB* boundingBox() const;
	Vec3 position() const;
	double surfaceArea() const;
	Vec3 randomPoint(Random* random) const;
	bool intersect(const Ray& ray, Intersection *intersection) const;

private:
	Vec3 _position;
	Vec3 _normal;
	Vertex _vertex_a, _vertex_b, _vertex_c;
	Vec3 _color;
	Vec3 _emission;
	Vec3 _edge_ab, _edge_ac;
	Material* _material;
	AABB* _bounding_box;
	double _surface_area;
};

}//namespace LtRenderer
