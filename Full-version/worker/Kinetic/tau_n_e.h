#include "GammaSimulator.h"

template <typename T, typename RNGT>
inline T GammaSimulator<T, RNGT>::tau_n_e(T v)
{
    T sum = v + n_e_v_1;
    T alpha_n;
    if (sum != 0)
    {
        alpha_n = n_e_a_1 * sum / (1 - exp(-sum / n_e_a_2));
    }
    else
    {
        alpha_n = n_e_a_1 * n_e_a_2;
    }
    T beta_n = n_e_b_1 * exp(-(v + n_e_v_2) / n_e_b_2);
    return 1 / (alpha_n + beta_n);
}