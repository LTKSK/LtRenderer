#pragma once
#include "Vector.h"
#include "Random.h"
#include "MathLib.h"

namespace LtRenderer
{

class Material
{
protected:
    Vec3 _albedo;
    Vec3 _emission;

    inline Vec3 orientingNormal(Ray ray, Vec3 normal) const
    {
        return dot(normal, ray.direction()) < 0.0 ? normal : -normal;
    }

public:
    Material(Vec3& albedo, Vec3& emission): _albedo(albedo), _emission(emission){}
    ~Material(){}

    bool isEmissive()
    {
        return _emission.x() > 0.0 || _emission.y() > 0.0 || _emission.z() > 0.0;
    }

    Vec3 albedo()
    {
        return _albedo;
    }

    Vec3 emission()
    {
        return _emission;
    }
    // 反射したRayを返す関数.副作用として、attenuationも同時に計算する
    virtual Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, Vec3& attenuation, Random* random) const = 0;
    virtual std::string materialType() const = 0;
};

class Lambertion : public Material
{
    const std::string _material_type = "Lambertion";
public:
    Lambertion(Vec3& albedo, Vec3& emission) : Material(albedo, emission) {}
    ~Lambertion(){}

    Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, Vec3& attenuation, Random* random) const
    {
        Vec3 orienting_normal = orientingNormal(ray, normal);
        Vec3 u, v, w;
        w = orienting_normal;
        if (abs(w.x()) > 0.1)
        {
            u = normalize(cross(Vec3(0.0, 1.0, 0.0), w));
        }
        else
        {
            u = normalize(cross(Vec3(1.0, 0.0, 0.0), w));
        }
        v = cross(w, u);
        double r1  = D_PI * 2 * random->zeroToOneFloat();
        double r2  = random->zeroToOneFloat();
        double r2s = sqrt(r2);

        Vec3 diffuse_direction = normalize(u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1.0 - r2));

        attenuation *= _albedo;
        return Ray(hit_position + diffuse_direction*0.01f, diffuse_direction);
    }
    std::string materialType() const
    {
        return _material_type;
    }
};

class Metal : public Material
{
    const std::string _material_type = "Metal";
public:
    Metal(Vec3& albedo, Vec3& emission) : Material(albedo, emission){}
    Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, Vec3& attenuation, Random* random) const
    {
        Vec3 reflect_direction = normalize(reflect(ray.direction(), normal));
        attenuation *= _albedo;
        return Ray(hit_position+reflect_direction * 0.01f, reflect_direction);
    }
    std::string materialType() const
    {
        return _material_type;
    }
};

class Dielectric : public Material
{
    double _ior;
    const std::string _material_type = "Dierectric";
public:
    Dielectric(Vec3& albedo, Vec3& emission, double ior) : Material(albedo, emission), _ior(ior){}

    Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, Vec3& attenuation, Random* random) const
    {
        // rayが内側からオブジェクトに交差した場合にはnormalを反転させる
        Vec3 orienting_normal = dot(normal, ray.direction()) < 0.0 ? normal : -normal;
        bool into = dot(normal, orienting_normal) > 0.0;
        double air_ior = 1.0;
        double ior_ratio = into ? air_ior / _ior : _ior / air_ior;
        double dot_dn = dot(ray.direction(), orienting_normal);
        double D = 1.0 - ior_ratio * ior_ratio * (1.0 - dot_dn * dot_dn);

        //Dが0.0より小さい場合、屈折後のベクトルとnormalの角度が90を超えているので全反射として扱う
        if (D < 0.0)
        {
            attenuation *= _albedo;
            Vec3 reflect_direction = normalize(reflect(ray.direction(), orienting_normal));
            return Ray(hit_position + reflect_direction * 0.01f, reflect_direction);
        }

        //schlickの近似を用いて、屈折光の運ぶ光の割合を求める
        double f0 = pow2((_ior - air_ior) / (_ior + air_ior));
        double cosine = into ? -dot_dn : dot(ray.direction(), normal);
        double re = f0 + (1 - f0) * pow5(1 - cosine);
        double reflect_prob = 1.0 - re;
        //屈折時の放射輝度の変化は屈折率の比の二乗
        double ior_ratio2 = pow2(ior_ratio);
        double tr = reflect_prob * ior_ratio2;
        double prob = re * 0.5 + 0.25;
        //結果を乱数で分岐する
        if (prob > random->zeroToOneFloat())
        {
            //反射方向のサンプリング
            attenuation *= _albedo * re / prob;
            Vec3 reflect_direction = normalize(reflect(ray.direction(), orienting_normal));
            return Ray(hit_position + reflect_direction * 0.01f, reflect_direction);
        }
        //屈折方向のサンプリング
        attenuation *= _albedo * tr / (1.0 - prob);
        Vec3 refract_direction = normalize((ray.direction() * ior_ratio) - (orienting_normal * (ior_ratio * dot_dn + sqrt(D))));
        return Ray(hit_position + refract_direction * 0.01f, refract_direction);
    }
    std::string materialType() const
    {
        return _material_type;
    }
};

}//namespace LtRenderer
