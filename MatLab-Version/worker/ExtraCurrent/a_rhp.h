#pragma once

#include "ExtraCurrent.h"

#include <math.h>   // for exp() and cosh()


// Right-hand part of the activation variable "a" ODE
template <typename T, typename RNGT>
inline T ExtraCurrent<T, RNGT>::a_rhp(T a, T v)
{  
    T a_inf = 1 / ( 1 + exp(-s_a * ( v - v_a ) ) );
    
    T k_a = 1 / t_a * cosh( s_a * ( v - v_a ) / 2 );
    
    T a_inc = k_a * ( -a + a_inf );

    return a_inc;
}