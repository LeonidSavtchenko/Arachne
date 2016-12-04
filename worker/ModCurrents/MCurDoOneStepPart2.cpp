#include "ModCurrent.h"
#include <omp.h>

// Second part of the midpoint method step
template <typename T, typename RNGT>
void ModCurrent<T, RNGT>::DoOneStepPart2(int iter, const DistVector<T> &v, const DistVector<T> &v_tmp)
{
    I = p_AllModCurrents->DoOneStepPart2(v, v_tmp, dt05);

    I.CopySliceToMatrixColumn(watchedModCurrentI, iter + 1, watchedModCurrentIdx);
}
