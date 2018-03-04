#pragma once
#include "Constant.h"
#include "Ray.h"
#include "Material.h"
#include "Intersection.h"
#include "Collision.h"

namespace LtRenderer
{

class Mesh 
{
	
public:
	Mesh() {}
	virtual inline Vec3 position() const = 0;
	virtual inline bool intersect(const Ray& ray, Intersection *intersection) const = 0;
	virtual inline Material* material() const = 0;
	virtual inline AABB* boundingBox() const = 0;
	virtual inline Vec3 randomPoint(Random* random) const = 0;
	virtual double surfaceArea() const = 0;
};

class Sphere : public Mesh
{
	Vec3 position_;
	double radius_;
	double surface_area_;
	Material* material_;
	AABB* bounding_box_;
public:
	Sphere() {}
	Sphere(Vec3 center, double radius, Material* material): position_(center), radius_(radius), material_(material)
	{
		double max_x = center.x() + radius;
		double max_y = center.y() + radius;
		double max_z = center.z() + radius;
		double min_x = center.x() - radius;
		double min_y = center.y() - radius;
		double min_z = center.z() - radius;
		bounding_box_ = new AABB(Vec3(max_x, max_y, max_z), Vec3(min_x, min_y, min_z));
		surface_area_ =  (4.0 * D_PI * radius * radius);
	}
	~Sphere()
	{
		delete bounding_box_;
		delete material_;
	}
	
	Material* material() const
	{
		return material_;
	}

	AABB* boundingBox() const
	{
		return bounding_box_;
	}

	Vec3 position() const
	{
		return position_;
	}

	double surfaceArea() const
	{
		return surface_area_;
	}

	Vec3 randomPoint(Random* random) const
	{
		const auto r1 = D_PI * 2.0 * random->zeroToOneFloat();
		const auto r2 = random->zeroToOneFloat();
		const auto r2s = sqrt(r2);
		return position_ + (radius_ + D_HIT_MIN) * Vec3(sqrt(1.0 - r2 * r2) * cos(r1), sqrt(1.0 - r2 * r2) * sin(r1), r2);
	}

	inline bool intersect(const Ray& ray, Intersection *intersection) const
	{
		auto p = position_ - ray.origin();
		auto b = dot(p, ray.direction());
		auto det = b * b - dot(p, p) + radius_ * radius_;
		if (det >= 0.0)
		{
			auto sqrt_det = sqrt(det);
			auto t1 = b - sqrt_det;
			auto t2 = b + sqrt_det;
			//t1,t2�̂����ꂩ���A�Փˎ��̒l������l���傫���A�Ȃ�������hit���Ă��鑼��object��菬�����ꍇ��true
			if (t1 > D_HIT_MIN && intersection->t >= t1)
			{
				intersection->setNormal(normalize(ray.pointAtParameter(t1) - position_));
				intersection->setPosition(ray.pointAtParameter(t1));
				intersection->setMaterial(material_);
				intersection->t = t1;
				return true;
			}
			if (t2 > D_HIT_MIN && intersection->t >= t2)
			{
				intersection->setNormal(normalize(ray.pointAtParameter(t2) - position_));
				intersection->setPosition(ray.pointAtParameter(t2));
				intersection->setMaterial(material_);
				intersection->t = t2;
				return true;
			}
		}
		return false;		
	}
};

class Triangle : public Mesh
{
	Vec3 position_;
	Vec3 normal_;
	Vec3 vertex_a_, vertex_b_, vertex_c_;
	Vec3 color_;
	Vec3 emission_;
	Vec3 edge_ab_, edge_ac_;
	Material* material_;
	AABB* bounding_box_;
	double surface_area_;

public:
	Triangle() : Mesh() {};
	Triangle(const Vec3& vertex_a,
			 const Vec3& vertex_b,
			 const Vec3& vertex_c,
			 Material* material) :
		vertex_a_(vertex_a),
		vertex_b_(vertex_b),
		vertex_c_(vertex_c),
		material_(material)
	{
		edge_ab_ = vertex_b_ - vertex_a_;
		edge_ac_ = vertex_c_ - vertex_a_;

		position_ = Vec3((vertex_a_.x() + vertex_b_.x() + vertex_c_.x()) / 3,
						 (vertex_a_.y() + vertex_b_.y() + vertex_c_.y()) / 3,
						 (vertex_a_.z() + vertex_b_.z() + vertex_c_.z()) / 3);

		//�O�p�`���\������3���_�̊e���̍ő�ŏ�����AABB���\�z����
		double max_x = std::fmax(vertex_a_.x(), std::fmax(vertex_b_.x(), vertex_c_.x()));
		double max_y = std::fmax(vertex_a_.y(), std::fmax(vertex_b_.y(), vertex_c_.y()));
		double max_z = std::fmax(vertex_a_.z(), std::fmax(vertex_b_.z(), vertex_c_.z()));
		double min_x = std::fmin(vertex_a_.x(), std::fmin(vertex_b_.x(), vertex_c_.x()));
		double min_y = std::fmin(vertex_a_.y(), std::fmin(vertex_b_.y(), vertex_c_.y()));
		double min_z = std::fmin(vertex_a_.z(), std::fmin(vertex_b_.z(), vertex_c_.z()));
		bounding_box_ = new AABB(Vec3(max_x, max_y, max_z),
								 Vec3(min_x, min_y, min_z));

		Vec3 cross_vec = cross(edge_ab_, edge_ac_);
		normal_ = normalize(cross_vec);
		surface_area_ = cross_vec.length() / 2.0;
	};
	~Triangle()
	{
		delete bounding_box_;
		delete material_;
	};

	Material* material() const
	{
		return material_;
	}

	AABB* boundingBox() const
	{
		return bounding_box_;
	}

	Vec3 position() const
	{
		return position_;
	}

	double surfaceArea() const
	{
		return surface_area_;
	}

	Vec3 randomPoint(Random* random) const
	{
		const auto rnd = random->zeroToOneFloat();
		return edge_ab_ * rnd + edge_ab_ * (1.0 - rnd);
	}

	//�s�񎮎Z�o�֐�
	//�N�������̌����ŁA�O�p�`��̌����ʒu�����߂�ۂɎg�p����
	/*
	��̓I�Ȍ�����
	x + y + z = 9
	2x + 3y -2z = 5
	3x -y + z = 7
	�Ƃ����A�����������������ۂɁA�e�ϐ��̌W�����x�N�g���Ƃ��Ă܂Ƃ߂āA
	(1, 2, 3)*x + (1,3,-1)*y + (1,-2,1)*z = (9,5,7)�Ƃ���B���̎�x�̌W����a�Ƃ���Bb,c�����l�ł���
	x = det(d, b, c)/det(a, b, c)�ŋ��܂�B

	*/
	double det(Vec3 a, Vec3 b, Vec3 c) const
	{
		return (a.x()*b.y()*c.z()) + (a.y()*b.z()*c.x()) + (a.z()*b.x()*c.y()) - (a.x()*b.z()*c.y()) - (a.y()*b.x()*c.z()) - (a.z()*b.y()*c.x());
	}

	/*
	�O�p�`�̓����蔻��́Avertex0 + edgeA * u + edgeB * v = ray.org + ray.dir * t�ŋ��߂���
	������edgeA,B�́A���ꂼ��O�p�`�̈꒸�_�ł���vertex0����c��2�̒��_�ւƐL�т�����
	��L�̎����A�W���̖������̂Ƃ�����̂Ő�������ƁA
	edgeA * u + edgeB * v - ray.dir * t = ray.org - vertex0
	�����ŁA�W��u,v,t���N�������̌������g�p���ċ��߂�
	*/
	inline bool intersect(const Ray& ray, Intersection *intersection) const
	{
		//�����̊֌W�ŁAray�͋t����
		Vec3 inv_raydir = normalize(Vec3(-ray.direction()));
		//(a,b,c)���̌�̌v�Z�̋��ʕ���ɂȂ镔��.
		//a,b,c�ō����̐ςŁA���̌�o�ė���̐ς������āAuvt�����߂�
		double denominator = det(edge_ab_, edge_ac_, inv_raydir);
		//���C�����ʂƕ��s�łȂ��ꍇ
		if (denominator > 0)
		{
			Vec3 vertex_a_pos = ray.origin() - vertex_a_;

			//(d,b,c)
			double u = det(vertex_a_pos, edge_ac_, inv_raydir) / denominator;
			//u���͈͊O�������牽�����Ȃ�
			if (0.0 <= u && u <= 1.0)
			{
				//(a,d,c)
				double v = det(edge_ab_, vertex_a_pos, inv_raydir) / denominator;
				if (0.0 <= v && u + v <= 1.0)
				{
					//(a,b,d)
					double t = det(edge_ab_, edge_ac_, vertex_a_pos) / denominator;
					if (t < 0 || t > intersection->t)
					{
						return false;
					}
					intersection->setNormal(normal_);
					intersection->setPosition(ray.pointAtParameter(t));
					intersection->setMaterial(material());
					intersection->t = t;
					return true;
				}
			}
		}
		return false;
	}
};

}//namespace LtRenderer
