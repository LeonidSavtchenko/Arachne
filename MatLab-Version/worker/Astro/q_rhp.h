#include "AstroNet.h"

// q ODE right-hand part
template <typename T, typename RNGT>
inline T AstroNet<T, RNGT>::q_rhp(const T &Ca, const T &ip3, const T &q)
{
    T alpha = a2 * d2 * (ip3 + d1) / (ip3 + d3);
    T beta = a2 * Ca;
    T q_inc = alpha * (1 - q) - beta * q;
    return q_inc;
}