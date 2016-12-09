#include "ExtraCurrent.h"
#include <omp.h>

// First part of the midpoint method step
template <typename T, typename RNGT>
void ExtraCurrent<T, RNGT>::DoOneStepPart1(const DistVector<T> &v)
{
    using namespace DistEnv;

    int localLength = a.localLength; 

    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);
   
    for (int idx = startIdx; idx < endIdx; idx++)
    {
        a_tmp[idx] = a[idx] + dt05 * a_rhp(a[idx], v[idx]);
        b_tmp[idx] = b[idx] + dt05 * b_rhp(b[idx], v[idx]);

        I[idx] = ComputeExtraCurrent(a_tmp[idx], b_tmp[idx], v[idx]);
    }
}
