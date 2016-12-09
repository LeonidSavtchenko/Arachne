#include "ModCurrent.h"
#include <omp.h>

// First part of the midpoint method step
template <typename T, typename RNGT>
void ModCurrent<T, RNGT>::DoOneStepPart1(const DistVector<T> &v)
{
    I = p_AllModCurrents->DoOneStepPart1(v, dt05);
}
