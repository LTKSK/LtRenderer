#pragma once
#include <vector>
#include <algorithm>
#include "Mesh.h"
#include "Collision.h"
#include "Intersection.h"
#include "Vector.h"


namespace LtRenderer
{

class BVHNode
{
    //BVHNodeの当たり判定
    AABB* bounding_box;
    //二分木でchildを保持する
    std::vector<Mesh *> leaf_polygons;
    std::vector<BVHNode *> children;

public:
	BVHNode();
	~BVHNode();

	void addChild(BVHNode* child);
	BVHNode* leftChild();
	BVHNode* rightChild();
	bool isLeaf();
	void setLeafPolygons(std::vector<Mesh *> polygons);
	std::vector<Mesh *> leafPolygons();
	AABB* boundingBox();
	bool intersect(const Ray& ray);
};

class BVH
{
    std::vector<Mesh *> polygons;
    BVHNode* root_bvh;
public:
	BVH();
	~BVH();
	void createAABBfromPolygons(AABB *aabb, std::vector<Mesh *> polygons);
	void makeLeaf(BVHNode* bvh, std::vector<Mesh *> leaf_polygons);
	bool intersect(const Ray& ray, Intersection* intersection, BVHNode* bvh_node = nullptr);
	void build(std::vector<Mesh *> polygons, BVHNode* bvh = nullptr);
};

}//namespace LtRenderer
