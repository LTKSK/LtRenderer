#pragma once
#include "Vector.h"
#include "Random.h"
#include "MathLib.h"

namespace LtRenderer 
{

class Material
{
protected:
	Vec3 albedo_;
	Vec3 emission_;

	inline Vec3 orientingNormal(Ray ray, Vec3 normal) const
	{
		return dot(normal, ray.direction()) < 0.0 ? normal : -normal;
	}

public:
	Material(Vec3& albedo, Vec3& emission): albedo_(albedo), emission_(emission){}
	~Material(){}
	
	bool isEmissive()
	{
		return emission_.x() > 0.0f || emission_.y() > 0.0f || emission_.z() > 0.0f;
	}

	Vec3 albedo()
	{
		return albedo_;
	}

	Vec3 emission()
	{
		return emission_;
	}
	// ���˂���Ray��Ԃ��֐�.����p�Ƃ��āAattenuation�������Ɍv�Z����
	virtual Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, Vec3& attenuation, Random* random) const = 0;
	virtual std::string materialType() const = 0;
};

class Lambertion : public Material
{
	const std::string material_type_ = "Lambertion";
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
		float r1  = F_PI * 2 * random->zeroToOneFloat();
		float r2  = random->zeroToOneFloat();
		float r2s = sqrt(r2);

		Vec3 diffuse_direction = normalize(u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1.0 - r2));

		attenuation *= albedo_;
		return Ray(hit_position + diffuse_direction*0.01f, diffuse_direction);
	}
	std::string materialType() const
	{
		return material_type_;
	}
};

class Metal : public Material
{
	const std::string material_type_ = "Metal";
public:
	Metal(Vec3& albedo, Vec3& emission) : Material(albedo, emission){}
	Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, Vec3& attenuation, Random* random) const
	{
		Vec3 reflect_direction = normalize(reflect(ray.direction(), normal));
		attenuation *= albedo_;
		return Ray(hit_position+reflect_direction * 0.01f, reflect_direction);
	}
	std::string materialType() const
	{
		return material_type_;
	}
};

class Dielectric : public Material
{
	float ior_;
	const std::string material_type_ = "Dierectric";
public:
	Dielectric(Vec3& albedo, Vec3& emission, float ior) : Material(albedo, emission), ior_(ior){}
	
	Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, Vec3& attenuation, Random* random) const
	{
		// ray����������I�u�W�F�N�g�Ɍ��������ꍇ�ɂ�normal�𔽓]������
		Vec3 orienting_normal = dot(normal, ray.direction()) < 0.0 ? normal : -normal;
		bool into = dot(normal, orienting_normal) > 0.0;
		float air_ior = 1.0;
		float ior_ratio = into ? air_ior / ior_ : ior_ / air_ior;
		float dot_dn = dot(ray.direction(), orienting_normal);
		float D = 1.0 - ior_ratio * ior_ratio * (1.0 - dot_dn * dot_dn);

		//D��0.0��菬�����ꍇ�A���܌�̃x�N�g����normal�̊p�x��90�𒴂��Ă���̂őS���˂Ƃ��Ĉ���
		if (D < 0.0)
		{
			attenuation *= albedo_;
			Vec3 reflect_direction = normalize(reflect(ray.direction(), orienting_normal));
			return Ray(hit_position + reflect_direction * 0.01f, reflect_direction);
		}

		//schlick�̋ߎ���p���āA���܌��̉^�Ԍ��̊��������߂�
		float f0 = pow2((ior_ - air_ior) / (ior_ + air_ior));
		float cosine = into ? -dot_dn : dot(ray.direction(), normal);
		float re = f0 + (1 - f0) * pow5(1 - cosine);
		float reflect_prob = 1.0 - re;
		//���܎��̕��ˋP�x�̕ω��͋��ܗ��̔�̓��
		float ior_ratio2 = pow2(ior_ratio);
		float tr = reflect_prob * ior_ratio2;
		float prob = re * 0.5 + 0.25;
		//���ʂ𗐐��ŕ��򂷂�
		if (prob > random->zeroToOneFloat())
		{
			//���˕����̃T���v�����O
			attenuation *= albedo_ * re / prob;
			Vec3 reflect_direction = normalize(reflect(ray.direction(), orienting_normal));
			return Ray(hit_position + reflect_direction * 0.01f, reflect_direction);
		}
		//���ܕ����̃T���v�����O
		attenuation *= albedo_ * tr / (1.0 - prob);
		Vec3 refract_direction = normalize((ray.direction() * ior_ratio) - (orienting_normal * (ior_ratio * dot_dn + sqrt(D))));
		return Ray(hit_position + refract_direction * 0.01f, refract_direction);
	}
	std::string materialType() const
	{
		return material_type_;
	}
};

}//namespace LtRenderer
