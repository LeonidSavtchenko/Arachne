#include "AstroNet.h"

template <typename T, typename RNGT>
void AstroNet<T, RNGT>::CorrectReleaseProbability(std::vector<BoostedBernoulliDistribution<T, RNGT>> &rvg, int iter)
{
    using namespace DistEnv;
    
    Ca.Gather(Ca_buf);
    int length = Ca_buf.length;
    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(length, myThread);
    int endIdx = GetThreadChunkStartIdx(length, myThread + 1);
    
    T n = 30;
    static T k = (1 - p_basic) / (n - 1);
    for (int idx = startIdx; idx < endIdx; idx++)
    {
        T p;
        if (Ca_buf[idx] <= CaBA)
        {
            p = p_basic;
        }
        else if (Ca_buf[idx] >= n * CaBA)
        {
            p = 1;
        }
        else
        {
            // Ca = CaBA     => p = p_basic;
            // Ca = n * CaBA => p = 1.
            p = p_basic + k * (Ca_buf[idx] - CaBA) / Ca_buf[idx];
        }

        rvg[idx].SetP(p);
    }
    
    if (myRank == MASTER_RANK)
    {
        #pragma omp barrier
        #pragma omp master
        {
            for (int idx = 0; idx < watchedAstroNum; idx++)
            {
                watchedProb.data[(iter + 1) * watchedAstroNum + idx] = rvg[watchedAstroIdx[idx]].GetP();
            }
        }
        #pragma omp barrier
    }
}