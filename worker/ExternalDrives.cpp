#include "GammaSimulator.h"
#include <omp.h>

// Compute deterministic external drive to E-cells
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ComputeExternalDrive_e(T t, bool firstCall)
{
    using namespace DistEnv;

    if (firstCall)
    {
        I_e.AssignZeros();
        /*
        int myThread = omp_get_thread_num();
        int startIdx = GetThreadChunkStartIdx(I_e.localLength, myThread);
        int endIdx = GetThreadChunkStartIdx(I_e.localLength, myThread + 1);
        for (int i = startIdx; i < endIdx; i++)
        {
            I_e[i] = T(0.5);
        }
        */
    }
    // (If the drive does not depend on time, then there is no need to update it after the first call)
}

// Compute deterministic external drive to I-cells
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ComputeExternalDrive_i(T t, bool firstCall)
{
    if (firstCall)
    {
        I_i.AssignZeros();
    }
    // (If the drive does not depend on time, then there is no need to update it after the first call)
}


// Here is an example on how to define more sophisticated deterministic external drive to E-cells as follows:
// I_e(t, idx) = sin(PI * idx / (num_e - 1)) * exp(-0.5 * t)
// with idx being end-to-end index of the distributed vector: idx = 0, 1, ..., num_e - 1.
/*
#define _USE_MATH_DEFINES
#include "math.h"
#include "DistEnv.h"

template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ComputeExternalDrive_e(T t, bool firstCall)
{
    using namespace DistEnv;

    // Get end-to-end index of the first element living on this rank
    int rankStartIdx = GetRankChunkStartIdx(num_e, myRank);

    // Get local index of the first element initialized by this thread
    int myThread = omp_get_thread_num();
    int threadStartIdx = GetThreadChunkStartIdx(I_e.localLength, myThread);

    // Get local index of the element after the last one initialized by this thread
    int threadEndIdx = GetThreadChunkStartIdx(I_e.localLength, myThread + 1);

    // Loop by local index: i_loc = threadStartIdx, threadStartIdx + 1, ..., threadEndIdx - 1.
    // Corresponding end-to-end index: i_e2e = rankStartIdx + i_loc.
    for (int i_loc = threadStartIdx, i_e2e = rankStartIdx + threadStartIdx;
        i_loc < threadEndIdx;
        i_loc++, i_e2e++)
    {
        I_e[i_loc] = sin(T(M_PI * i_e2e) / (num_e - 1)) * exp(T(-0.5) * t);
    }
}
*/