#include "GammaSimulator.h"

template <typename T, typename RNGT>
inline T GammaSimulator<T, RNGT>::tau_h_e(T v)
{
    T alpha_h = h_e_a_1 * exp(-(v + h_e_v_1) / h_e_a_2);
    T beta_h = h_e_b_1 / (1 + exp(-(v + h_e_v_2) / h_e_b_2));
    return 1 / (alpha_h + beta_h);
}