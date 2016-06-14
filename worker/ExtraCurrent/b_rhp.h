#pragma once

#include "ExtraCurrent.h"

#include <math.h>   // for exp() and cosh()


// Right-hand part of the inactivation variable "b" ODE
template <typename T, typename RNGT>
inline T ExtraCurrent<T, RNGT>::b_rhp(T b, T v)
{  
    T b_inf = 1 / ( 1 + exp( -s_b * ( v - v_b ) ) );
    
    T k_b = 1 / t_b * cosh( s_b * ( v - v_b ) / 2 );
    
    T b_inc = k_b * ( -b + b_inf );

    return b_inc;
}