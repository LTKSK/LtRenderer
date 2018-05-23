#pragma once
#include <cstdint>
#include <cmath>

namespace LtRenderer
{

class XorShift
{
    uint32_t x = 123456789;
    uint32_t y = 362436069;
    uint32_t z = 521288629;
    uint32_t w = 88675123;
public:
    XorShift() {}
    XorShift(const uint32_t seed)
    {
        if (seed > 0)
        {
            w = seed;
        }
    }

    inline uint32_t randomUint()
    {
        uint32_t t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        return w;
    }

    inline double zeroToOneDouble()
    {
        return static_cast<double>(randomUint()) / _UI32_MAX;
    }

    inline float zeroToOneFloat()
    {
        return static_cast<float>(randomUint()) / _UI32_MAX;
    }
};

typedef XorShift Random;

}//namespace LtRenderer
