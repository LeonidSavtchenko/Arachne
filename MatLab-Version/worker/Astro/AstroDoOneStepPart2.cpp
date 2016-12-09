#include "AstroNet.h"
#include <omp.h>
#include <math.h> // for exp() math function

// Second part of the midpoint method step
template <typename T, typename RNGT>
void AstroNet<T, RNGT>::DoOneStepPart2(int iter, DistVector<int> &lastSpikes_e, int lastNumSpikes_e)
{
    using namespace DistEnv;
    
    int myThread = omp_get_thread_num();
    int localLength = Ca_tmp.localLength;
    int startIdx = GetThreadChunkStartIdx(localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);
    auto mpiTypeTag = GetMpiDataType<T>();
    
    // Get left and right values of Ca from neighbour chunks
    #pragma omp barrier
    #pragma omp master
    {
        if (numRanks == 1)
        {
            minCa = Ca_tmp[localLength - 1];
            plCa = Ca_tmp[0];
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
            
            MPI_Send(Ca_tmp.localData, 1, mpiTypeTag, minRank, myRank, MPI_COMM_WORLD);
            MPI_Recv(&plCa, 1, mpiTypeTag, plRank, plRank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Barrier(MPI_COMM_WORLD);
            
            MPI_Send(Ca_tmp.localData + localLength - 1, 1, mpiTypeTag, plRank, myRank, MPI_COMM_WORLD);
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
            leftCa = Ca_tmp[idx - 1];
        }
        
        if (idx == localLength - 1)
        {
            rightCa = plCa;
        }
        else
        {
            rightCa = Ca_tmp[idx + 1];
        }
        
        T Jsyn = gs * (2 * Ca_tmp[idx] - leftCa - rightCa);
        
        Ca[idx] += dt * (Ca_rhp(Ca_tmp[idx], ip3_tmp[idx], q_tmp[idx]) - Jsyn);
        ip3[idx] += dt * ip3_rhp(ip3_tmp[idx], y_tmp[idx]);
        q[idx] += dt * q_rhp(Ca_tmp[idx], ip3_tmp[idx], q_tmp[idx]);

        y[idx] +=  - dt * y_tmp[idx] / t_in; 
    }
    
    // Remark: The "y" ODE does not depend on other ODEs and allows analytic solving.
    //         But we solve it numerically because calculation of explicit analytic solution
    //         would be affected by strong finite-precision arithmetics effects
    //         and would lead to progressively worsening performance.
    startIdx = GetThreadChunkStartIdx(lastNumSpikes_e, myThread);
    endIdx = GetThreadChunkStartIdx(lastNumSpikes_e, myThread + 1);
   
    T jumpKoef =  tau_spike / timeScale;
    for (int idx = startIdx; idx < endIdx; idx++)
    {
        y[lastSpikes_e[idx]] += jumpKoef;
    }
   
    // Save Ca and activation parameter "y" of watched astrocytes
    Ca.CopySliceToMatrixColumn(watchedAstroCa, iter + 1, watchedAstroIdx);
    y.CopySliceToMatrixColumn(watchedAstroY, iter + 1, watchedAstroIdx);

    if (gatherCaColormap && (iter + 1) % caColormapPeriodIter == 0)
    {
        // Save a column of Ca colormap
        int colIdx = (iter + 1) / caColormapPeriodIter;
        CaColormap.FillColFromVector(Ca, colIdx, true);
    }
}