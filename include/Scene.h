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
        // //上
        // objects.push_back(new Triangle(Vec3(0, 100, -50), Vec3(100, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
        // objects.push_back(new Triangle(Vec3(0, 100, 200), Vec3(100, 100, -50), Vec3(100, 100, 200), new Lambertion(Vec3(0.8, 0.3, 0.3), Vec3())));
        // //下
        // objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, -50), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
        // objects.push_back(new Triangle(Vec3(100, 0, 200), Vec3(100, 0, -50), Vec3(0, 0, 200), new Lambertion(Vec3(0.3, 0.3, 0.8), Vec3())));
        // //左
        // objects.push_back(new Triangle(Vec3(0, 0, 200), Vec3(0, 0, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // objects.push_back(new Triangle(Vec3(0, 0, -50), Vec3(0, 100, -50), Vec3(0, 100, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // //右
        // objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 0, -50), Vec3(100, 0, 200), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // objects.push_back(new Triangle(Vec3(100, 100, 200), Vec3(100, 100, -50), Vec3(100, 0, -50), new Lambertion(Vec3(0.8, 0.8, 0.8), Vec3())));
        // //奥
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
        // 逆変換の式はthita = acos(z/square())
        double thita = acos(dir.y());
        //xz平面の角度
        double phi = atan2(dir.z(), dir.x());
        if (phi < 0.0)
        {
            phi += D_PI * 2.0;
        }
        //角度をpixel座標に変換する

        //radian2degree rad * 180 / D_PI;
        //width or height / 360 1度辺りのpixel数
        //return Vec3();
        return _bg_image->pixelFromUV(thita / D_PI, phi / (D_PI * 2.0));
    }

    Vec3 samplingLightArea()
    {
        //ランダムなオブジェクトを一つ選んで
        //そのオブジェクトの表面上の点をランダムサンプリングする
        return Vec3();
    }

    Vec3 nextEventEstimation(const Intersection* intersection, Random* random)
    {
        const auto& light = randomLightObject(random);
        const auto  sampled_light_pos = light->randomPoint(random);

        // ライトベクトル.
        auto light_dir = sampled_light_pos - intersection->position();
        auto light_dist2 = dot(light_dir, light_dir);
        light_dir = normalize(light_dir);
        auto light_normal = normalize(sampled_light_pos - light->position());

        auto dot0 = dot(intersection->normal(), light_dir);
        auto dot1 = dot(light_normal, -light_dir);

        //寄与が取れる場合.
        //衝突点とlightそれぞれのdotが0以上である(裏から当たったりしていない),且つ
        //距離二乗がライトの半径の二乗より大きい
        if (dot0 >= 0.0 && dot1 >= 0.0)
        {
            Ray shadow_ray(intersection->position(), light_dir);
            Intersection nee_intersection;
            const bool is_hit = bvhIntersectScene(shadow_ray, &nee_intersection);
            // ライトのみと衝突した場合のみ寄与を取る.
            if (is_hit && nee_intersection.material()->isEmissive())
            {
                //距離の二乗に反比例する減衰
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