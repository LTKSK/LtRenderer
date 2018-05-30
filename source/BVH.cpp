#include "BVH.h"


namespace LtRenderer
{

BVHNode::BVHNode()
{
	bounding_box = new AABB();
}

BVHNode::~BVHNode()
{
	delete bounding_box;
	leaf_polygons.clear();
	leaf_polygons.shrink_to_fit();
	children.clear();
	children.shrink_to_fit();
}

void BVHNode::addChild(BVHNode* child)
{
	if (children.size() == 2)
	{
		return;
	}
	children.push_back(child);
}

BVHNode* BVHNode::leftChild()
{
	return children[0];
}

BVHNode* BVHNode::rightChild()
{
	return children[1];
}

bool BVHNode::isLeaf()
{
	return children.empty();
}

void BVHNode::setLeafPolygons(std::vector<Mesh *> polygons)
{
	leaf_polygons = polygons;
}

std::vector<Mesh *> BVHNode::leafPolygons()
{
	return leaf_polygons;
}

AABB* BVHNode::boundingBox()
{
	return bounding_box;
}

bool BVHNode::intersect(const Ray& ray)
{
	return bounding_box->intersect(ray);
}

BVH::BVH()
{
	root_bvh = new BVHNode();
}

BVH::~BVH()
{
	polygons.clear();
	polygons.shrink_to_fit();
}

void BVH::createAABBfromPolygons(AABB *aabb, std::vector<Mesh *> polygons)
{
	for (auto polygon : polygons)
	{
		aabb->merge(polygon->boundingBox());
	}
	return;
}

void BVH::makeLeaf(BVHNode* bvh, std::vector<Mesh *> leaf_polygons)
{
	createAABBfromPolygons(bvh->boundingBox(), leaf_polygons);
	bvh->setLeafPolygons(leaf_polygons);
}

bool BVH::intersect(const Ray& ray, Intersection* intersection, BVHNode* bvh_node)
{
	//初回ループ用の分岐
	if (bvh_node == nullptr)
	{
		bvh_node = root_bvh;
	}

	//交差しなければ帰る
	if (!bvh_node->intersect(ray))
	{
		return false;
	}

	//末端でなければ再帰的にchildを探索する
	if (!bvh_node->isLeaf())
	{
		bool left_result = intersect(ray, intersection, bvh_node->leftChild());
		bool right_result = intersect(ray, intersection, bvh_node->rightChild());
		return left_result || right_result;
	}

	//leafの場合、内部の全てのpolygonと衝突判定を行い、その結果を返す
	bool is_hit = false;
	for (auto polygon : bvh_node->leafPolygons())
	{
		is_hit |= polygon->intersect(ray, intersection);
	}
	return is_hit;
}

void BVH::build(std::vector<Mesh *> polygons, BVHNode* bvh)
{
	BVHNode* bvh_node = bvh;
	if (bvh_node == nullptr)
	{
		bvh_node = root_bvh;
	}
	//polygonからrootになるAABBを作成し、rootのnodeに設定する
	createAABBfromPolygons(bvh_node->boundingBox(), polygons);
	//polygonsを分割するindexを格納する
	//どの軸で分割するのが最も良いかを保存する0:x, 1:y, 2:z
	int best_axis = -1;
	//この値は、分割を続けるのかそれともleafとしてしまうのかの基準になる
	double best_cost = D_HIT_MAX;

	double polygons_max_distance = 0.0;
	for (int axis = 0; axis < 3; ++axis)
	{
		//各軸毎にsortを行う
		std::sort(polygons.begin(), polygons.end(),
			[axis](const Mesh* a, const Mesh* b)
		{
			switch (axis)
			{
			case  0: return a->boundingBox()->center().x() < b->boundingBox()->center().x();
			case  1: return a->boundingBox()->center().y() < b->boundingBox()->center().y();
			case  2: return a->boundingBox()->center().z() < b->boundingBox()->center().z();
			default: return a->boundingBox()->center().x() < b->boundingBox()->center().x();
			}
		});
		double distance = fabs((polygons[0]->position() - polygons[polygons.size() - 1]->position()).length());
		if (distance > polygons_max_distance)
		{
			polygons_max_distance = distance;
			best_axis = axis;
		}
	}

	std::sort(polygons.begin(), polygons.end(),
		[best_axis](const Mesh* a, const Mesh* b)
	{
		switch (best_axis)
		{
		case  0: return a->boundingBox()->center().x() < b->boundingBox()->center().x();
		case  1: return a->boundingBox()->center().y() < b->boundingBox()->center().y();
		case  2: return a->boundingBox()->center().z() < b->boundingBox()->center().z();
		default: return a->boundingBox()->center().x() < b->boundingBox()->center().x();
		}
	});

	BVHNode* left_bvh_node = new BVHNode();
	BVHNode* right_bvh_node = new BVHNode();
	bvh_node->addChild(left_bvh_node);
	bvh_node->addChild(right_bvh_node);
	//polygonsを分割
	int split_index = int(polygons.size()) / 2;
	std::vector<Mesh *> left_polygons(polygons.begin(), polygons.begin() + split_index);
	std::vector<Mesh *> right_polygons(polygons.begin() + split_index, polygons.end());

	//todo 下の4はマジックナンバーなので定数化
	int max_leaf_polygon_count = 4;
	if (left_polygons.size() <= max_leaf_polygon_count)
	{
		makeLeaf(left_bvh_node, left_polygons);
	}
	else
	{
		build(left_polygons, left_bvh_node);
	}

	if (right_polygons.size() <= max_leaf_polygon_count)
	{
		makeLeaf(right_bvh_node, right_polygons);
	}
	else
	{
		build(right_polygons, right_bvh_node);
	}
}

}//namespace LtRenderer