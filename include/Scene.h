#pragma once
#include <vector>
#include "Mesh.h"
#include "BVH.h"

namespace LtRenderer
{

class Scene
{
	std::vector<Mesh *> objects;
	std::vector<Mesh *> light_objects_;
	BVHTree* bvh_tree_;
	double pdf_;
public:
	Scene()
	{
		//lights 
		objects.push_back(new Sphere(Vec3(50.0, 100, 100.0), 8.5, new Lambertion(Vec3(0.0), Vec3(7.0))));
		//objects.push_back(new Sphere(Vec3(10.0, 100, 100.0), 8.5, new Lambertion(Vec3(0.0), Vec3(12.0))));
		//��
		objects.push_back(new Triangle(Vec3(0, 100, -50), Vec3(100, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
		objects.push_back(new Triangle(Vec3(0, 100, 200), Vec3(100, 100, -50), Vec3(100, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
		//��
		objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, -50), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(100, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, 200), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
		//��
		objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(0, 0, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(0, 0, -50), Vec3(0, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		//�E
		objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 0, -50), Vec3(100, 0, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 100, -50), Vec3(100, 0, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		//��
		objects.push_back(new Triangle(Vec3(100, 100, -50), Vec3(0, 100, -50), Vec3(100, 0, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		objects.push_back(new Triangle(Vec3(0, 0, -50), Vec3(100, 0, -50), Vec3(0, 100, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));

		//spheres
		objects.push_back(new Sphere(Vec3(27.0, 60.5, 78.0), 16.5, new Metal(Vec3(0.0, 0.99, 0.0), Vec3())));
		objects.push_back(new Sphere(Vec3(40.0, 16.5, 147.0), 16.5, new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
		objects.push_back(new Sphere(Vec3(73.0, 40.5, 100.0), 16.5, new Dielectric(Vec3(1.0), Vec3(), 1.5)));

		bvh_tree_ = new BVHTree();
		printf("BVH Build start\n");
		bvh_tree_->build(objects);
		printf("BVH Build end\n\n");

		double light_area = 0.0;
		for (auto obj : objects)
		{
			if (obj->material()->isEmissive())
			{
				light_objects_.push_back(obj);
				light_area += obj->surfaceArea();
			}
		}
		pdf_ = 1.0 / light_area;
	}

	~Scene()
	{
		objects.clear();
		objects.shrink_to_fit();
		delete bvh_tree_;
	}

	const auto randomLightObject(Random* random)
	{
		int random_index = random->randomUint() % light_objects_.size();
		return light_objects_[random_index];
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

	Vec3 samplingIBL()
	{
		return Vec3();
	}

	Vec3 samplingLightArea()
	{
		//�����_���ȃI�u�W�F�N�g����I���
		//���̃I�u�W�F�N�g�̕\�ʏ�̓_�������_���T���v�����O����
		return Vec3();
	}

	Vec3 nextEventEstimation(const Intersection* intersection, Random* random)
	{
		const auto& light = randomLightObject(random);
		const auto  sampled_light_pos = light->randomPoint(random);

		// ���C�g�x�N�g��.
		auto light_dir = sampled_light_pos - intersection->position();

		// ���C�g�ւ̋�����2��
		auto light_dist2 = dot(light_dir, light_dir);

		// ���K��.
		light_dir = normalize(light_dir);

		// ���C�g�̖@���x�N�g��.
		auto light_normal = normalize(sampled_light_pos - light->position());

		auto dot0 = dot(intersection->normal(), light_dir);
		auto dot1 = dot(light_normal, -light_dir);

		//��^������ꍇ.
		//�Փ˓_��light���ꂼ���dot��0�ȏ�ł���(�����瓖�������肵�Ă��Ȃ�)
		//����
		//������悪���C�g�̔��a�̓����傫��
		if (dot0 >= 0.0 && dot1 >= 0.0)
		{
			Ray shadow_ray(intersection->position(), light_dir);
			Intersection nee_intersection;
			const bool is_hit = bvhIntersectScene(shadow_ray, &nee_intersection);
			// ���C�g�݂̂ƏՓ˂����ꍇ�̂݊�^�����.
			if (is_hit && nee_intersection.material()->isEmissive())
			{
				//�����̓��ɔ���Ⴗ�錸��
				auto G = dot0 * dot1 / light_dist2;
				return nee_intersection.material()->emission() * (intersection->material()->albedo() / D_PI) * G * pdf_;
			}
		}
		else
		{
			//TODO IBL sampling
		}
		return Vec3();
	}

};

}//namespace LtRenderer