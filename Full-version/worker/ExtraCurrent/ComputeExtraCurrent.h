#pragma once

#include "ExtraCurrent.h"

#include <math.h>   // for pow()


template <typename T, typename RNGT>
inline T ExtraCurrent<T, RNGT>::ComputeExtraCurrent(T a, T b, T v)
{ 
    if (g == 0)
    {
        return 0;
    }

    return -g * pow(a, p) * pow(b, q) * (v - v_r);
}
