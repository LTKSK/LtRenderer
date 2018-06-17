#pragma once
#include "Vector.h"


namespace LtRenderer
{

class Vertex
{
public:
	Vertex(Vec3 position, Vec2 uv);
	~Vertex();
    Vec3 position() const;
    Vec2 uv() const;

private:
	const Vec3 _position;
	const Vec2 _uv;
};

}//namespace LtRenderer