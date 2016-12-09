#include "GammaSimulator.h"

template <typename T, typename RNGT>
inline T GammaSimulator<T, RNGT>::m_i_inf(T v)
{
    T alpha_m;
    T sum = v + m_i_v_1;
    if (sum != 0)
    {
        alpha_m = m_i_a_1 * sum / (1 - exp(-sum / m_i_a_2));
    }
    else
    {
        alpha_m = m_i_a_1 * m_i_a_2;
    }
    T beta_m = m_i_b_1 * exp(-(v + m_i_v_2) / m_i_b_2);
    return alpha_m / (alpha_m + beta_m);
}