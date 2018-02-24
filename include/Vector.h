#pragma once
#include <iostream>
#include <stdlib.h>

namespace LtRenderer
{

class Vec3
{
	float elements_[3];
public:
	Vec3()
	{
		elements_[0] = elements_[1] = elements_[2] = 0.0;
	}

	Vec3(const float value)
	{
		elements_[0] = elements_[1] = elements_[2] = value;
	}

	Vec3(float element0, float element1, float element2)
	{
		elements_[0] = element0;
		elements_[1] = element1;
		elements_[2] = element2;
	}

	inline float x() const { return elements_[0]; }
	inline float y() const { return elements_[1]; }
	inline float z() const { return elements_[2]; }
	inline float operator [](int i) const
	{
		return elements_[i];
	}

	inline Vec3 operator - () const
	{
		return Vec3(-elements_[0], -elements_[1], -elements_[2]);
	}

	inline Vec3 operator + (const Vec3& value) const
	{
		return Vec3(elements_[0]+value.x(), elements_[1] + value.y(), elements_[2] + value.z());
	}
	inline Vec3 operator - (const Vec3& value) const
	{
		return Vec3(elements_[0] - value.x(), elements_[1] - value.y(), elements_[2] - value.z());
	}
	inline Vec3 operator * (const Vec3& value) const
	{
		return Vec3(elements_[0] * value.x(), elements_[1] * value.y(), elements_[2] * value.z());
	}
	inline Vec3 operator / (const Vec3& value) const
	{
		return Vec3(elements_[0] / value.x(), elements_[1] / value.y(), elements_[2] / value.z());
	}
	inline Vec3 operator * (const float value) const
	{
		return Vec3(elements_[0] * value, elements_[1] * value, elements_[2] * value);
	}
	inline Vec3 operator / (const float value) const
	{
		return Vec3(elements_[0] / value, elements_[1] / value, elements_[2] / value);
	}

	inline Vec3& operator += (const Vec3 &value)
	{
		elements_[0] += value.x(); 
		elements_[1] += value.y();
		elements_[2] += value.z();
		return *this;	
	}
	inline Vec3& operator -= (const Vec3 &value)
	{
		elements_[0] -= value.x();
		elements_[1] -= value.y();
		elements_[2] -= value.z();
		return *this;
	}
	inline Vec3& operator *= (const Vec3 &value) 
	{
		elements_[0] *= value.x();
		elements_[1] *= value.y();
		elements_[2] *= value.z();
		return *this;
	}
	inline Vec3& operator /= (const Vec3 &value)
	{
		elements_[0] /= value.x();
		elements_[1] /= value.y();
		elements_[2] /= value.z();
		return *this;
	}
	inline Vec3& operator *= (const float value)
	{
		elements_[0] *= value;
		elements_[1] *= value;
		elements_[2] *= value;
		return *this;
	}
	inline Vec3& operator /= (const float value)
	{
		elements_[0] /= value;
		elements_[1] /= value;
		elements_[2] /= value;
		return *this;
	}

	inline float length() const
	{
		return sqrt(elements_[0]*elements_[0] + elements_[1]*elements_[1] + elements_[2]*elements_[2]);
	}
};

inline Vec3 operator * (const float value1, const Vec3& value2)
{
	return Vec3(value2.x() * value1, value2.y() * value1, value2.z() * value1);
}

inline Vec3 normalize(const Vec3& value)
{
	return value / value.length();
}
inline float dot(const Vec3 value1, const Vec3 value2)
{
	return value1.x() * value2.x() + value1.y() * value2.y() + value1.z() * value2.z();
}
inline Vec3 cross(const Vec3 value1, const Vec3 value2)
{
	return Vec3(value1.y()*value2.z() - value1.z()*value2.y(),
				value1.x()*value2.z() - value1.z()*value2.x(),
				value1.x()*value2.y() - value1.y()*value2.x());
}

inline Vec3 reflect(const Vec3& value, const Vec3& normal)
{
	return value - normal * 2.0 * dot(value, normal);
}

}//namespace LtRenderer
