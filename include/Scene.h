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
    std::vector<Mesh *> _objects;
    std::vector<Mesh *> _light_objects;
    BVH* _bvh;
    double _pdf;
    Image* _bg_image;
public:
    Scene()
    {
        auto loader = ObjectLoader();
		_objects = loader.load();
        //lights
        _objects.push_back(new Sphere(Vec3(0, 75.0, 0.0), 2.5, new Lambertion(Vec3(0.0), Vec3(7.0))));
        // //��
        // objects.push_back(new Triangle(Vec3(0, 100, -50), Vec3(100, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
        // objects.push_back(new Triangle(Vec3(0, 100, 200), Vec3(100, 100, -50), Vec3(100, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
        // //��
        // objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, -50), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
        // objects.push_back(new Triangle(Vec3(100, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, 200), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
        // //��
        // objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(0, 0, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // objects.push_back(new Triangle(Vec3(0, 0, -50), Vec3(0, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // //�E
        // objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 0, -50), Vec3(100, 0, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 100, -50), Vec3(100, 0, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // //��
        // objects.push_back(new Triangle(Vec3(100, 100, -50), Vec3(0, 100, -50), Vec3(100, 0, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // objects.push_back(new Triangle(Vec3(0, 0, -50), Vec3(100, 0, -50), Vec3(0, 100, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));

        // //spheres
        _bvh = new BVH();
        printf("BVH Build start\n");
        _bvh->build(_objects);
        printf("BVH Build end\n\n");
        double light_area = 0.0;
        for (auto obj : _objects)
        {
            if (obj->material()->isEmissive())
            {
                _light_objects.push_back(obj);
                light_area += obj->surfaceArea();
            }
        }
        _pdf = 1.0 / light_area;
        _bg_image = new Image("bg.bmp");
    }

    ~Scene()
    {
        _objects.clear();
        _objects.shrink_to_fit();
        delete _bvh;
        delete _bg_image;
    }

    const auto randomLightObject(Random* random)
    {
        int random_index = random->randomUint() % _light_objects.size();
        return _light_objects[random_index];
    }

    inline bool bvhIntersectScene(const Ray& ray, Intersection* intersection)
    {
        return _bvh->intersect(ray, intersection);
    }

    Vec3 samplingIBL(const Vec3 dir)
    {
        // �t�ϊ��̎���thita = acos(z/square())
        double thita = acos(dir.y());
        //xz���ʂ̊p�x
        double phi = atan2(dir.z(), dir.x());
        if (phi < 0.0)
        {
            phi += D_PI * 2.0;
        }
        //�p�x��pixel���W�ɕϊ�����

        //radian2degree rad * 180 / D_PI;
        //width or height / 360 1�x�ӂ��pixel��
        //return Vec3();
        return _bg_image->pixelFromUV(thita / D_PI, phi / (D_PI * 2.0));
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
        auto light_dist2 = dot(light_dir, light_dir);
        light_dir = normalize(light_dir);
        auto light_normal = normalize(sampled_light_pos - light->position());

        auto dot0 = dot(intersection->normal(), light_dir);
        auto dot1 = dot(light_normal, -light_dir);

        //��^������ꍇ.
        //�Փ˓_��light���ꂼ���dot��0�ȏ�ł���(�����瓖�������肵�Ă��Ȃ�),����
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
                return nee_intersection.material()->emission() * (intersection->material()->albedo() / D_PI) * G * _pdf;
            }
        }
        else
        {
            //TODO IBL sampling?
        }
        return Vec3();
    }

};

}//namespace LtRenderer