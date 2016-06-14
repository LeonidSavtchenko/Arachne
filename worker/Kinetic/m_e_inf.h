#include "GammaSimulator.h"

template <typename T, typename RNGT>
inline T GammaSimulator<T, RNGT>::m_e_inf(T v)
{
    T alpha_m;
    T sum = v + m_e_v_1;
    if (sum != 0)
    {
        alpha_m = m_e_a_1 * sum / (1 - exp(-sum / m_e_a_2));
    }
    else
    {
        alpha_m = m_e_a_1 * m_e_a_2;
    }
    
    T beta_m;
    sum += (m_e_v_2 - m_e_v_1);
    if (sum != 0)
    {
        beta_m = m_e_b_1 * sum / (exp(sum / m_e_b_2) - 1);
    }
    else
    {
        beta_m = m_e_b_1 * m_e_b_2;
    }
    
    return alpha_m / (alpha_m + beta_m);
}