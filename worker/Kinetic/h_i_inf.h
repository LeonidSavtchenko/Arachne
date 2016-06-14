#include "GammaSimulator.h"

template <typename T, typename RNGT>
inline T GammaSimulator<T, RNGT>::h_i_inf(T v)
{
    T alpha_h = h_i_a_1 * exp(-(v + h_i_v_1) / h_i_a_2);
    T beta_h = h_i_b_1 / (exp(h_i_b_2 * (v + h_i_v_2)) + 1);
    return alpha_h / (alpha_h + beta_h);
} 