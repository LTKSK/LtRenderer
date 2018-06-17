#include "Vertex.h"


namespace LtRenderer
{

Vertex::Vertex(Vec3 position, Vec2 uv): _position(position), _uv(uv) {}

Vertex::~Vertex() {}

Vec3 Vertex::position() const
{
    return _position;
}

Vec2 Vertex::uv() const
{
    return _uv;
}

}//namespace LtRenderer