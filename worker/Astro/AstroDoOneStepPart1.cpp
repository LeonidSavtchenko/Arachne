#include "AstroNet.h"
#include <omp.h>

// First part of the midpoint method step
template <typename T, typename RNGT>
void AstroNet<T, RNGT>::DoOneStepPart1(DistVector<int> &lastSpikes_e, int lastNumSpikes_e)
{
    using namespace DistEnv;
    
    int myThread = omp_get_thread_num();
    int localLength = Ca.localLength;
    int startIdx = GetThreadChunkStartIdx(localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);
    auto mpiTypeTag = GetMpiDataType<T>();
    
    // Get left and right values of Ca from neighbour chunks
    #pragma omp barrier
    #pragma omp master
    {
        if (numRanks == 1)
        {
            minCa = Ca[localLength - 1];
            plCa = Ca[0];
        }
        else
        {
            int minRank = myRank - 1;
            int plRank = myRank + 1;
            if (minRank < 0)
            {
                minRank = numRanks - 1;
            }
            if (plRank == numRanks)
            {
                plRank = 0;
            }
            
            MPI_Send(Ca.localData, 1, mpiTypeTag, minRank, myRank, MPI_COMM_WORLD);
            MPI_Recv(&plCa, 1, mpiTypeTag, plRank, plRank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Barrier(MPI_COMM_WORLD);
            
            MPI_Send(Ca.localData + localLength - 1, 1, mpiTypeTag, plRank, myRank, MPI_COMM_WORLD);
            MPI_Recv(&minCa, 1, mpiTypeTag, minRank, minRank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    #pragma omp barrier
    
    for (int idx = startIdx; idx < endIdx; idx++)
    {
        T leftCa, rightCa;
        
        if (idx == 0)
        {
            leftCa = minCa;
        }
        else
        {
            leftCa = Ca[idx - 1];
        }
        
        if (idx == localLength - 1)
        {
            rightCa = plCa;
        }
        else
        {
            rightCa = Ca[idx + 1];
        }
        
        T Jsyn = gs * (2 * Ca[idx] - leftCa - rightCa);
        
        Ca_tmp[idx] = Ca[idx] + dt05 * (Ca_rhp(Ca[idx], ip3[idx], q[idx]) - Jsyn);
        ip3_tmp[idx] = ip3[idx] + dt05 * ip3_rhp(ip3[idx], y[idx]);
        q_tmp[idx] = q[idx] + dt05 * q_rhp(Ca[idx], ip3[idx], q[idx]);
        
        y_tmp[idx] = y[idx] * (1 - dt05 / t_in);
    }
}