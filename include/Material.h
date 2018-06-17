#pragma once
#include "Vector.h"
#include "Ray.h"
#include "Random.h"
#include "Constant.h"
#include "MathLib.h"
#include "Image.h"

namespace LtRenderer
{

class Material
{
public:
	Material(Vec3& albedo, Vec3& emission);
	~Material();

	bool isEmissive();
	Vec3 albedo();
	Vec3 emission();
	void setAlbedoTexture(Image* texture);
    // ”½Ë‚µ‚½Ray‚ğ•Ô‚·ŠÖ”.•›ì—p‚Æ‚µ‚ÄAattenuation‚à“¯‚ÉŒvZ‚·‚é
    virtual Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, const Vec2 uv, Vec3& attenuation, Random* random) const = 0;
    virtual std::string materialType() const = 0;

protected:
	Vec3 _albedo;
	Vec3 _emission;
	Image* _albedo_texture;
	Vec3 orientingNormal(Ray ray, Vec3 normal) const;
};

class Lambertion : public Material
{
public:
	Lambertion(Vec3& albedo, Vec3& emission);
	~Lambertion();
	virtual Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, const Vec2 uv, Vec3& attenuation, Random* random) const;
	virtual std::string materialType() const;

private:
	const std::string _material_type = "Lambertion";
};

class Metal : public Material
{
public:
	Metal(Vec3& albedo, Vec3& emission);
	~Metal();
	virtual Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, const Vec2 uv, Vec3& attenuation, Random* random) const;
	virtual std::string materialType() const;

private:
	const std::string _material_type = "Metal";
};

class Dielectric : public Material
{
public:
	Dielectric(Vec3& albedo, Vec3& emission, double ior);
	~Dielectric();
	virtual Ray scatter(const Ray& ray, const Vec3& hit_position, const Vec3& normal, const Vec2 uv, Vec3& attenuation, Random* random) const;
	virtual
		std::string materialType() const;
private:
	double _ior;
	const std::string _material_type = "Dierectric";
};

}//namespace LtRenderer
