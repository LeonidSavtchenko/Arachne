#include "GammaSimulator.h"

#define _USE_MATH_DEFINES
#include <math.h>   // M_PI, cos
#include "MathUtils.h"

// Compute potentials phi_e or phi_i in observation points.
// The phi matrix lives on master rank only.
// The following global variables are used additionally:
// opNum, opRadii, opAngles, buffer.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ComputePotObsPts
(
    LocalDenseMatrix<T, RNGT> &phi,
    const DistVector<T> &gts1,
    const DistVector<T> &gts2,
    T radius,
    int iter
)
{
    using namespace DistEnv;
    
    int myThread = omp_get_thread_num();
    int num = gts1.length;
    int localNum = gts1.localLength;
    int rankStartIdx = GetRankChunkStartIdx(num, myRank);
    int threadStartIdx = GetThreadChunkStartIdx(localNum, myThread);
    int threadEndIdx = GetThreadChunkStartIdx(localNum, myThread + 1);
    T *phiData = phi.data;
    auto mpiTypeTag = GetMpiDataType<T>();
    T pi = (T)M_PI;
    
    for (int j = 0; j < opNum; j++)
    {
        T c1 = radius * radius + opRadii[j] * opRadii[j];
        T c2 = 2 * radius * opRadii[j];
        
        // Compute sum for each thread
        buffer[myThread] = 0;
        for (int idx_loc = threadStartIdx, idx_e2e = rankStartIdx + threadStartIdx;
            idx_loc < threadEndIdx;
            idx_loc++, idx_e2e++)
        {
            buffer[myThread] += (gts1[idx_loc] + gts2[idx_loc]) / sqrt(c1 - c2 * cos(opAngles[j] - 2 * pi * idx_e2e / num));
        }
        
        #pragma omp barrier

        #pragma omp master
        {
            // Compute sum by all threads of this process
            T localSum = 0;
            for (int i = 0; i < numThreads; i++)
            {
                localSum += buffer[i];
            }
            
            T &phiValue = phiData[opNum * iter + j];
            
            if (myRank == MASTER_RANK)
            {
                phiValue = localSum;
            }

            // Add sums from all other processes
            for (int i = 1; i < numRanks; i++)
            {
                if (myRank == i)
                {
                    MPI_Send(&localSum, 1, mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD);
                }
                else if (myRank == MASTER_RANK)
                {
                    MPI_Recv(&localSum, 1, mpiTypeTag, i, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
                    phiValue += localSum;
                }

                MPI_Barrier(MPI_COMM_WORLD);
            }
            
            if (myRank == MASTER_RANK)
            {
                phiValue /= (4 * pi * electrolCond);
                //!!assert(!isNaN(phiValue));
            }
            
            MPI_Barrier(MPI_COMM_WORLD);
        }

        #pragma omp barrier
    }
}