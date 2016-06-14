#include "UniformSCM.h"
#include "Containers/DistDenseMatrix.h"
#include "DistEnv.h"
#include "GetTypeTagUtils.h"

#include <omp.h>
#include <stdint.h> // uint64_t

#include <limits>   // quiet_NaN

using namespace std;
using namespace DistEnv;


// Compute product taking history into scope
template <typename T, typename RNGT>
void UniformSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    using namespace DistEnv;
    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round(n, 4);
    int totalNumCols = this->numCols;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
        
    const T value = this->value;
    const T *xData = X.data;
    T *yData = y.localData;
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(m, myThread);
    int yEndIdx = GetThreadChunkStartIdx(m, myThread + 1);
    
    // Loop by y (destination cell index)
    for (int i = yStartIdx; i < yEndIdx; i++)
    {
        yData[i] = 0;
        int j0 = div_round((startIdx + i) * n, totalNumCols);
        for (int pre = -cutPoint; pre <= cutPoint; pre++)
        {
            // Source cell index
            int j = this->GetRowIdx(j0, pre);
                
            bool s = this->rvg[j](releaseGenStates[myThread]);
            if (s)
            {
                // Get index of the past iteration we take into scope now
                int delay = 1 + abs(pre);
                int pastIdx = currentIdx - delay * delta;
                if (pastIdx < 0)
                {
                    pastIdx += max_delay;
                }

                // Take into account influence of cell j to cell i
                yData[i] += xData[pastIdx * n + j];
            }
        }
        yData[i] *= value;
    }
}

// Overload function which computes product taking history into scope
template <typename T, typename RNGT>
void UniformSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    using namespace DistEnv;

    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round(n, 4);
    int totalNumCols = this->numCols;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
        
    const T value = this->value;
    const T *xData = X.localData;
    T *yData = y.localData;
    
    T *xColData = this->x_tmp.data;
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(m, myThread);
    int yEndIdx = GetThreadChunkStartIdx(m, myThread + 1);
    
    for (int i = yStartIdx; i < yEndIdx; i++) y[i] = 0;
    
    // Get index of the past iteration we take into scope now
    for (int pre = -cutPoint; pre <= cutPoint; pre++)
    {
        int delay = 1 + abs(pre);
        int pastIdx = currentIdx - delay * delta;
        if (pastIdx < 0)
        {
            pastIdx += max_delay;
        }
        
        int num_rank = GetRankNumber(max_delay, pastIdx);
        int local_idx = n * (pastIdx - GetRankChunkStartIdx(max_delay, num_rank));
    
        #pragma omp barrier
        #pragma omp master
        {
            if (num_rank == myRank)
            {
                memcpy(xColData, xData + local_idx, n * sizeof(T));
            }    
            MPI_Bcast(xColData, n, this->mpiTypeTag, num_rank, MPI_COMM_WORLD);
        }
        #pragma omp barrier
        
        //Loop by y (destination cell index)
        for (int i = yStartIdx; i < yEndIdx; i++)
        {
            // Source cell index
            int j0 = div_round((startIdx + i) * n, totalNumCols);
            int j = this->GetRowIdx(j0, pre);
                
            bool s = this->rvg[j](releaseGenStates[myThread]);
            if (s)
            {
                // Take into account influence of cell j to cell i
                yData[i] += value * xColData[j];
            }
        }
    }
}

// Default constructor
template <typename T, typename RNGT>
UniformSCM<T, RNGT>::UniformSCM()
{
    this->value = numeric_limits<T>::quiet_NaN();
}

// Custom constructor.
// numRows, numCols - number of rows and total number of columns,
// value - constant value, matrix is filled with
template <typename T, typename RNGT>
UniformSCM<T, RNGT>::UniformSCM(int numRows, int numCols, LocalVector<T> releaseProb, T value)
{
    using namespace DistEnv;
    
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->mpiTypeTag = GetMpiDataType<T>();
    this->value = value;
    this->x_tmp = LocalVector<T>(numRows, AllocMode::allRanksNew);
    this->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
    for (int j = 0; j < numRows; j++)
    {
        this->rvg[j] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[j]);
    }
}

// Destructor
template <typename T, typename RNGT>
UniformSCM<T, RNGT>::~UniformSCM()
{
    this->value = numeric_limits<T>::quiet_NaN();
    // (Base class destructor does)
}

template
class UniformSCM<float, std::mt19937>;

template
class UniformSCM<double, std::mt19937>;

template
class UniformSCM<double, std::mt19937_64>;
