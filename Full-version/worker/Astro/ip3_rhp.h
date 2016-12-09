#include "AstroNet.h"

// IP3 ODE right-hand part
template <typename T, typename RNGT>
inline T AstroNet<T, RNGT>::ip3_rhp(const T &ip3, const T &y)
{
    T ip3_inc = (ip3star - ip3) / tip3 + rip3 * y;
    return ip3_inc;
}