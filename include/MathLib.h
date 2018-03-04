#pragma once

namespace LtRenderer
{
	inline double pow2(const double value) { return value * value; }
	inline double pow3(const double value) { return value * value * value; }
	inline double pow4(const double value) { return value * value * value * value; }
	inline double pow5(const double value) { return value * value * value * value * value; }
	inline double saturate(const double value) { return value > 1.0 ? 1.0 : value < 0.0 ? 0.0 : value; }
	inline double min(const double value1, double value2) { return value1 > value2 ? value2 : value1; }
	inline double max(const double value1, double value2) { return value1 > value2 ? value1 : value2; }
}