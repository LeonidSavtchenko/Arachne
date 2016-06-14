#include "GammaSimulator.h"

template <typename T, typename RNGT>
inline T GammaSimulator<T, RNGT>::tau_n_i(T v)
{
    T sum = v + n_i_v_1;
    T alpha_n;
    if (sum != 0)
    {
        alpha_n = n_i_a_1 * sum / (exp(n_i_a_2 * sum) - 1);
    }
    else
    {
        alpha_n = n_i_a_1 / n_i_a_2;
    }
    T beta_n = n_i_b_1 * exp(-(v + n_i_v_2) / n_i_b_2);
    T tau_n_i = 1 / (alpha_n + beta_n);
    return tau_n_i / phi;
}