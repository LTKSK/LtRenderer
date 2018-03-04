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
	Vec3 _position;
	double _radius;
	double _surface_area;
	Material* _material;
	AABB* _bounding_box;
public:
	Sphere() {}
	Sphere(Vec3 center, double radius, Material* material): _position(center), _radius(radius), _material(material)
	{
		double max_x = center.x() + radius;
		double max_y = center.y() + radius;
		double max_z = center.z() + radius;
		double min_x = center.x() - radius;
		double min_y = center.y() - radius;
		double min_z = center.z() - radius;
		_bounding_box = new AABB(Vec3(max_x, max_y, max_z), Vec3(min_x, min_y, min_z));
		_surface_area =  (4.0 * D_PI * radius * radius);
	}
	~Sphere()
	{
		delete _bounding_box;
		delete _material;
	}
	
	Material* material() const
	{
		return _material;
	}

	AABB* boundingBox() const
	{
		return _bounding_box;
	}

	Vec3 position() const
	{
		return _position;
	}

	double surfaceArea() const
	{
		return _surface_area;
	}

	Vec3 randomPoint(Random* random) const
	{
		const auto r1 = D_PI * 2.0 * random->zeroToOneFloat();
		const auto r2 = random->zeroToOneFloat();
		const auto r2s = sqrt(r2);
		return _position + (_radius + D_HIT_MIN) * Vec3(sqrt(1.0 - r2 * r2) * cos(r1), sqrt(1.0 - r2 * r2) * sin(r1), r2);
	}

	inline bool intersect(const Ray& ray, Intersection *intersection) const
	{
		auto p = _position - ray.origin();
		auto b = dot(p, ray.direction());
		auto det = b * b - dot(p, p) + _radius * _radius;
		if (det >= 0.0)
		{
			auto sqrt_det = sqrt(det);
			auto t1 = b - sqrt_det;
			auto t2 = b + sqrt_det;
			//t1,t2のいずれかが、衝突時の値が閾値より大きく、なおかつ既にhitしている他のobjectより小さい場合にtrue
			if (t1 > D_HIT_MIN && intersection->t >= t1)
			{
				intersection->setNormal(normalize(ray.pointAtParameter(t1) - _position));
				intersection->setPosition(ray.pointAtParameter(t1));
				intersection->setMaterial(_material);
				intersection->t = t1;
				return true;
			}
			if (t2 > D_HIT_MIN && intersection->t >= t2)
			{
				intersection->setNormal(normalize(ray.pointAtParameter(t2) - _position));
				intersection->setPosition(ray.pointAtParameter(t2));
				intersection->setMaterial(_material);
				intersection->t = t2;
				return true;
			}
		}
		return false;		
	}
};

class Triangle : public Mesh
{
	Vec3 _position;
	Vec3 _normal;
	Vec3 _vertex_a, _vertex_b, _vertex_c;
	Vec3 _color;
	Vec3 _emission;
	Vec3 _edge_ab, _edge_ac;
	Material* _material;
	AABB* _bounding_box;
	double _surface_area;

public:
	Triangle() : Mesh() {};
	Triangle(const Vec3& vertex_a,
			 const Vec3& vertex_b,
			 const Vec3& vertex_c,
			 Material* material) :
		_vertex_a(vertex_a),
		_vertex_b(vertex_b),
		_vertex_c(vertex_c),
		_material(material)
	{
		_edge_ab = _vertex_b - _vertex_a;
		_edge_ac = _vertex_c - _vertex_a;

		_position = Vec3((_vertex_a.x() + _vertex_b.x() + _vertex_c.x()) / 3,
						 (_vertex_a.y() + _vertex_b.y() + _vertex_c.y()) / 3,
						 (_vertex_a.z() + _vertex_b.z() + _vertex_c.z()) / 3);

		//三角形を構成する3頂点の各軸の最大最小からAABBを構築する
		double max_x = std::fmax(_vertex_a.x(), std::fmax(_vertex_b.x(), _vertex_c.x()));
		double max_y = std::fmax(_vertex_a.y(), std::fmax(_vertex_b.y(), _vertex_c.y()));
		double max_z = std::fmax(_vertex_a.z(), std::fmax(_vertex_b.z(), _vertex_c.z()));
		double min_x = std::fmin(_vertex_a.x(), std::fmin(_vertex_b.x(), _vertex_c.x()));
		double min_y = std::fmin(_vertex_a.y(), std::fmin(_vertex_b.y(), _vertex_c.y()));
		double min_z = std::fmin(_vertex_a.z(), std::fmin(_vertex_b.z(), _vertex_c.z()));
		_bounding_box = new AABB(Vec3(max_x, max_y, max_z),
								 Vec3(min_x, min_y, min_z));

		Vec3 cross_vec = cross(_edge_ab, _edge_ac);
		_normal = normalize(cross_vec);
		_surface_area = cross_vec.length() / 2.0;
	};
	~Triangle()
	{
		delete _bounding_box;
		delete _material;
	};

	Material* material() const
	{
		return _material;
	}

	AABB* boundingBox() const
	{
		return _bounding_box;
	}

	Vec3 position() const
	{
		return _position;
	}

	double surfaceArea() const
	{
		return _surface_area;
	}

	Vec3 randomPoint(Random* random) const
	{
		const auto rnd = random->zeroToOneFloat();
		return _edge_ab * rnd + _edge_ab * (1.0 - rnd);
	}

	//行列式算出関数
	//クラメルの公式で、三角形上の交差位置を求める際に使用する
	/*
	具体的な公式は
	x + y + z = 9
	2x + 3y -2z = 5
	3x -y + z = 7
	という連立方程式があった際に、各変数の係数をベクトルとしてまとめて、
	(1, 2, 3)*x + (1,3,-1)*y + (1,-2,1)*z = (9,5,7)とする。この時xの係数をaとする。b,cも同様である
	x = det(d, b, c)/det(a, b, c)で求まる。

	*/
	double det(Vec3 a, Vec3 b, Vec3 c) const
	{
		return (a.x()*b.y()*c.z()) + (a.y()*b.z()*c.x()) + (a.z()*b.x()*c.y()) - (a.x()*b.z()*c.y()) - (a.y()*b.x()*c.z()) - (a.z()*b.y()*c.x());
	}

	/*
	三角形の当たり判定は、vertex0 + edgeA * u + edgeB * v = ray.org + ray.dir * tで求められる
	ここでedgeA,Bは、それぞれ三角形の一頂点であるvertex0から残り2つの頂点へと伸びた線分
	上記の式を、係数の無いものとあるもので整理すると、
	edgeA * u + edgeB * v - ray.dir * t = ray.org - vertex0
	ここで、係数u,v,tをクラメルの公式を使用して求める
	*/
	inline bool intersect(const Ray& ray, Intersection *intersection) const
	{
		//公式の関係で、rayは逆向き
		Vec3 inv_raydir = normalize(Vec3(-ray.direction()));
		//(a,b,c)この後の計算の共通分母になる部分.
		//a,b,cで作られる体積で、この後出て来る体積を割って、uvtを求める
		double denominator = det(_edge_ab, _edge_ac, inv_raydir);
		//レイが平面と並行でない場合
		if (denominator > 0)
		{
			Vec3 vertex_a_pos = ray.origin() - _vertex_a;

			//(d,b,c)
			double u = det(vertex_a_pos, _edge_ac, inv_raydir) / denominator;
			//uが範囲外だったら何もしない
			if (0.0 <= u && u <= 1.0)
			{
				//(a,d,c)
				double v = det(_edge_ab, vertex_a_pos, inv_raydir) / denominator;
				if (0.0 <= v && u + v <= 1.0)
				{
					//(a,b,d)
					double t = det(_edge_ab, _edge_ac, vertex_a_pos) / denominator;
					if (t < 0 || t > intersection->t)
					{
						return false;
					}
					intersection->setNormal(_normal);
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
