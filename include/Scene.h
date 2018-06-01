#pragma once
#include <vector>
#include <iostream>
#include "Mesh.h"
#include "BVH.h"
#include "Image.h"
#include "ObjectLoader.h"

namespace LtRenderer
{

class Scene
{
public:
	Scene();
	~Scene();
	const auto randomLightObject(Random* random);
	bool intersectScene(const Ray& ray, Intersection* intersection);
	Vec3 samplingIBL(const Vec3 dir);
	Vec3 samplingLightArea();
	Vec3 nextEventEstimation(const Intersection* intersection, Random* random);

private:
	std::vector<Mesh *> _objects;
	std::vector<Mesh *> _light_objects;
	BVH* _bvh;
	double _pdf;
	Image* _bg_image;
};

}//namespace LtRenderer