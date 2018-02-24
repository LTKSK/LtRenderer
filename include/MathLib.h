#pragma once

namespace LtRenderer
{
	inline float pow2(const float value) { return value * value; }
	inline float pow3(const float value) { return value * value * value; }
	inline float pow4(const float value) { return value * value * value * value; }
	inline float pow5(const float value) { return value * value * value * value * value; }
	inline float saturate(const float value) { return value > 1.0 ? 1.0 : value < 0.0 ? 0.0 : value; }
	inline float min(const float value1, float value2) { return value1 > value2 ? value2 : value1; }
	inline float max(const float value1, float value2) { return value1 > value2 ? value1 : value2; }
}