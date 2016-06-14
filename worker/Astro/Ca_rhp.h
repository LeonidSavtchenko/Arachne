#include "AstroNet.h"

// Ca ODE right-hand part except of the "Jsyn" term
template <typename T, typename RNGT>
inline T AstroNet<T, RNGT>::Ca_rhp(const T &Ca, const T &ip3, const T &q)
{
    T m_inf = ip3 / (ip3 + d1);
    T n_inf = Ca / (Ca + d5);

    T Jc = c1 * v1 * pow(m_inf * n_inf * q, 3) * (Ca - Ca_ER);
    T Jl = c1 * v2 * (Ca - Ca_ER);
    T Jp = v3 * Ca * Ca / (k3 * k3 + Ca * Ca);

    T Ca_inc = -Jc - Jl - Jp;

    return Ca_inc;
}