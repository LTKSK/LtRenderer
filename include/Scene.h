#pragma once
#include <vector>
#include "Mesh.h"
#include "BVH.h"

namespace LtRenderer
{

class Scene
{
	std::vector<Mesh *> objects;
	BVHTree* bvh_tree_;
public:
	Scene()
	{
		//light object
		objects.push_back(new Sphere(Vec3(50.0, 16.5, 100.0), 8.5, new Lambertion(Vec3(0.0), Vec3(7.0))));
		//上
		objects.push_back(new Triangle(Vec3(0, 100, -50), Vec3(100, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
		objects.push_back(new Triangle(Vec3(0, 100, 200), Vec3(100, 100, -50), Vec3(100, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
		//下
		objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, -50), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(100, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, 200), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
		//左
		objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(0, 0, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(0, 0, -50), Vec3(0, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		//右
		objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 0, -50), Vec3(100, 0, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 100, -50), Vec3(100, 0, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		//奥
		objects.push_back(new Triangle(Vec3(100, 100, -50), Vec3(0, 100, -50), Vec3(100, 0, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(0, 0, -50), Vec3(100, 0, -50), Vec3(0, 100, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));

		//spheres
		objects.push_back(new Sphere(Vec3(27.0, 60.5, 78.0), 16.5, new Metal(Vec3(0.0, 0.99, 0.0), Vec3())));
		objects.push_back(new Sphere(Vec3(73.0, 16.5, 47.0), 16.5, new Lambertion(Vec3(0.99, 0.99, 0.99), Vec3())));
		//objects.push_back(new Sphere(Vec3(73.0, 83.5, 100.0), 16.5, new Dielectric(Vec3(1.0), Vec3(), 1.5)));

		bvh_tree_ = new BVHTree();
		printf("BVH Build start\n");
		bvh_tree_->build(objects);
		printf("BVH Build end\n\n");
	}

	~Scene()
	{
		objects.clear();
		objects.shrink_to_fit();
		delete bvh_tree_;
	}

	const auto light_object()
	{
		return objects[0];
	}

	inline bool intersectScene(const Ray& ray, Intersection* intersection)
	{
		bool is_hit = false;
		for (auto count = 0; count < objects.size(); ++count)
		{
			if (objects[count]->intersect(ray, intersection))
			{
				is_hit = true;
			}
		}
		return is_hit;
	}

	inline bool bvhIntersectScene(const Ray& ray, Intersection* intersection)
	{
		return bvh_tree_->intersect(ray, intersection);
	}

	Vec3 nextEventEstimation(const Intersection* intersection, Random* random)
	{
		// TODO: ここを整理する
		const auto& light = light_object();
		const auto  light_pos = light->randomPoint(random);

		// ライトベクトル.
		auto light_dir = light_pos - intersection->position();

		// ライトへの距離の2乗
		auto light_dist2 = dot(light_dir, light_dir);

		// 正規化.
		light_dir = normalize(light_dir);

		// ライトの法線ベクトル.
		auto light_normal = normalize(light_pos - light->position());

		auto dot0 = dot(intersection->normal(), light_dir);
		auto dot1 = dot(light_normal, -light_dir);

		//寄与が取れる場合.
		//衝突点とlightそれぞれのdotが0以上である(裏から当たったりしていない)
		//且つ
		//距離二乗がライトの半径の二乗より大きい
		if (dot0 >= 0 && dot1 >= 0)
		{
			Ray shadow_ray(intersection->position(), light_dir);

			// シャドウレイを発射.
			Intersection nee_intersection;
			const bool is_hit = bvhIntersectScene(shadow_ray, &nee_intersection);
			// ライトのみと衝突した場合のみ寄与を取る.
			// TODO lightの判定は数値ではなくclassのtypeなどで判別するように
			if (is_hit && nee_intersection.material()->isEmissive())
			{
				//距離の二乗に反比例
				auto G = dot0 * dot1 / light_dist2;
				return nee_intersection.material()->emission() * (intersection->material()->albedo() / F_PI) * G / light->pdf();
			}
		}
		return Vec3();
	}

};

}//namespace LtRenderer