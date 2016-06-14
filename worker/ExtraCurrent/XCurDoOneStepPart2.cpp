#include "ExtraCurrent.h"
#include <omp.h>

// Second part of the midpoint method step
template <typename T, typename RNGT>
void ExtraCurrent<T, RNGT>::DoOneStepPart2(int iter, const DistVector<T> &v, const DistVector<T> &v_tmp)
{
    using namespace DistEnv;

    int localLength = a.localLength;

    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);

    for (int idx = startIdx; idx < endIdx; idx++)
    {
        T a_inc = a_rhp(a_tmp[idx], v_tmp[idx]);
        T b_inc = b_rhp(b_tmp[idx], v_tmp[idx]);

        a[idx] += dt * a_inc;
        b[idx] += dt * b_inc;

        I[idx] = ComputeExtraCurrent(a[idx], b[idx], v[idx]);
    }

    I.CopySliceToMatrixColumn(watchedExtraCurrentI, iter + 1, watchedExtraCurrentIdx);
}
