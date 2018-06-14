#include "Mesh.h"


namespace LtRenderer
{
Mesh::Mesh() {}
Mesh::~Mesh() {}

/*
Sphere
*/

Sphere::Sphere(Vec3 center, double radius, Material* material) : _position(center), _radius(radius), _material(material)
{
	double max_x = center.x() + radius;
	double max_y = center.y() + radius;
	double max_z = center.z() + radius;
	double min_x = center.x() - radius;
	double min_y = center.y() - radius;
	double min_z = center.z() - radius;
	_bounding_box = new AABB(Vec3(max_x, max_y, max_z), Vec3(min_x, min_y, min_z));
	_surface_area = (4.0 * D_PI * radius * radius);
}

Sphere::~Sphere()
{
	delete _bounding_box;
	delete _material;
}

Material* Sphere::material() const
{
	return _material;
}

AABB* Sphere::boundingBox() const
{
	return _bounding_box;
}

Vec3 Sphere::position() const
{
	return _position;
}

double Sphere::surfaceArea() const
{
	return _surface_area;
}

Vec3 Sphere::randomPoint(Random* random) const
{
	const auto r1 = D_PI * 2.0 * random->zeroToOneFloat();
	const auto r2 = random->zeroToOneFloat();
	const auto r2s = sqrt(r2);
	return _position + (_radius + D_HIT_MIN) * Vec3(sqrt(1.0 - r2 * r2) * cos(r1), sqrt(1.0 - r2 * r2) * sin(r1), r2);
}

bool Sphere::intersect(const Ray& ray, Intersection *intersection) const
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

/*
Triangle
*/

Triangle::Triangle(
	const Vec3& vertex_a,
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
	_surface_area = cross_vec.length() * 0.5;
	_normal = normalize(cross_vec);
}

Triangle::~Triangle()
{
	delete _bounding_box;
	delete _material;
}

Material* Triangle::material() const
{
	return _material;
}

AABB* Triangle::boundingBox() const
{
	return _bounding_box;
}

Vec3 Triangle::position() const
{
	return _position;
}

double Triangle::surfaceArea() const
{
	return _surface_area;
}

Vec3 Triangle::randomPoint(Random* random) const
{
	const auto rnd = random->zeroToOneFloat();
	return _edge_ab * rnd + _edge_ab * (1.0 - rnd);
}

inline bool Triangle::intersect(const Ray& ray, Intersection *intersection) const
{
	Vec3 p_vec = cross(ray.direction(), _edge_ac);
	double det = dot(_edge_ab, p_vec);
	if (fabs(det) <= 1e-8)
	{
		return false;
	}

	double inv_det = 1.0 / det;
	Vec3 t_vec = ray.origin() - _vertex_a;
	double u = dot(t_vec, p_vec) * inv_det;
	if (u < 0.0 || u > 1.0)
	{
		return false;
	}

	Vec3 q_vec = cross(t_vec, _edge_ab);
	double v = dot(ray.direction(), q_vec) * inv_det;
	if (v < 0.0 || u + v > 1.0)
	{
		return false;
	}
	
	double t = dot(_edge_ac, q_vec) * inv_det;
	if (t < 0.0 || t > intersection->t)
	{
		return false;
	}

	intersection->setNormal(_normal);
	intersection->setPosition(ray.pointAtParameter(t));
	intersection->setMaterial(material());
	intersection->t = t;
	return true;
}

}//namespace LtRenderer