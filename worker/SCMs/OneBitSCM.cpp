#include "OneBitSCM.h"
#include "DistEnv.h"
#include "DistributionWrapper.h"
#include "GetTypeTagUtils.h"

#include <omp.h>
#include <stdint.h> // int64_t, uint64_t

#include <limits>   // quiet_NaN
#include <iostream>

using namespace std;
using namespace DistEnv;


// Compute product taking history into scope
template <typename T, typename RNGT>
void OneBitSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round(n, 4);
    int totalNumCols = this->numCols;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
    T g_hat = this->value;

    int64_t nLong = n;
    int64_t totalNumColsLong = totalNumCols;
        
    const uint64_t *matData = this->localData;
    const T *xData = X.data;
    T *yData = y.localData;
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(m, myThread);
    int yEndIdx = GetThreadChunkStartIdx(m, myThread + 1);
    
    // Loop by y (destination cell index)
    for (int i = yStartIdx; i < yEndIdx; i++)
    {
        yData[i] = 0;
        int j0 = div_round((startIdx + i) * nLong, totalNumColsLong);
        for (int pre = -cutPoint; pre <= cutPoint; pre++)
        {
            // Source cell index
            int j = this->GetRowIdx(j0, pre);
                
            bool s = this->rvg[j](releaseGenStates[myThread]);
            if (s)
            {
                int numElem = j / 64;
                int shift = j % 64;
                if (matData[i * n / 64 + numElem] & (1ULL << shift))
                {
                    // Get index of the past iteration we take into scope now
                    int delay = 1 + abs(pre);
                    int pastIdx = currentIdx - delay * delta;
                    if (pastIdx < 0)
                    {
                        pastIdx += max_delay;
                    }

                    // Take into account influence of cell j to cell i
                    yData[i] += g_hat * xData[pastIdx * n + j];
                }
            }
        }
    }
}

// Overload function which computes product taking history into scope
template <typename T, typename RNGT>
void OneBitSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round(n, 4);
    int totalNumCols = this->numCols;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
    T g_hat = this->value;

    int64_t nLong = n;
    int64_t totalNumColsLong = totalNumCols;
        
    const uint64_t *matData = this->localData;
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
        
        // Loop by y (destination cell index)
        for (int i = yStartIdx; i < yEndIdx; i++)
        {
            // Source cell index
            int j0 = div_round((startIdx + i) * nLong, totalNumColsLong);
            int j = this->GetRowIdx(j0, pre);
                
            bool s = this->rvg[j](releaseGenStates[myThread]);
            if (s)
            {
                // Take into account influence of cell j to cell i
                int numElem = j / 64;
                int shift = j % 64;
                if (matData[i * n / 64 + numElem] & (1ULL << shift))
                {
                    yData[i] += g_hat * xColData[j];
                }
            }
        }
    }
}

// Default constructor
template <typename T, typename RNGT>
OneBitSCM<T, RNGT>::OneBitSCM()
{
    this->localData = nullptr;
    this->value = numeric_limits<T>::quiet_NaN();
}

template <typename T, typename RNGT>
OneBitSCM<T, RNGT>::OneBitSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, bool isOneType, const DistVector<int> &seeds)
{
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->matrix = DistDenseMatrix<uint64_t, RNGT>(numRows / 64, numCols);
    this->localData = matrix.localData;
    this->value = g_hat;
    this->x_tmp = LocalVector<T>(numRows, AllocMode::allRanksNew);
    this->mpiTypeTag = GetMpiDataType<T>();
    this->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
    for (int j = 0; j < numRows; j++)
    {
        this->rvg[j] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[j]);
    }
    
    LocalVector<T> weight = this->PrepareWeightVector(w_max, sigma, isOneType);
    
    #pragma omp parallel
    {
        OneBitSCMMultithreaded(weight, g_hat, seeds);
    }
}

// Internal part of the custom constructor
template <typename T, typename RNGT>
void OneBitSCM<T, RNGT>::OneBitSCMMultithreaded(const LocalVector<T> &weight, T g_hat, const DistVector<int> &seeds)
{
    using namespace DistEnv;
    
    int m = this->numRows;
    int n = this->localNumCols;
    int totalNumCols = this->numCols;
    uint64_t *data = this->localData;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
    int cutPoint = div_round(m, 4);

    int64_t mLong = m;
    int64_t totalNumColsLong = totalNumCols;
    
    int nw = weight.length;
    int c = (nw - 1) / 2;
    const T *w_data = weight.data;
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(n, myThread);
    int yEndIdx = GetThreadChunkStartIdx(n, myThread + 1);
    int mySeed = seeds[myThread];
    auto myState = RNGT(mySeed);
    
    auto maxValue = myState.max();  // Remark: Linux compiler does not allow syntax RNGT::max()
    typedef typename RNGT::result_type result_type;
    T boundary = T(maxValue);
    
    // Populate it with zeros at start
    memset(data + (m / 64) * yStartIdx, 0, (m / 64) * (yEndIdx - yStartIdx) * sizeof(uint64_t));
        
    for (int pre = -cutPoint; pre <= cutPoint; pre++)
    {
        for (int j = yStartIdx; j < yEndIdx; j++)
        {
            // Source cell index
            int i0 = div_round((startIdx + j) * mLong, totalNumColsLong);
            int i = this->GetRowIdx(i0, pre);
        
            int index = c + div_round(((j + startIdx) * (mLong - 1) - i * (totalNumColsLong - 1)) * (nw - 1), (mLong - 1) * (totalNumColsLong - 1));
            while (index < 0) index += nw;
            while (index > nw - 1) index -= nw;
            result_type randomValue = myState();
            if (randomValue / boundary < w_data[index])
            {
                int numElem = i / 64;
                int shift = i - numElem * 64;
                data[j * m / 64 + numElem] |= (1ULL << shift);
            }
        }
    }
}

// Destructor
template <typename T, typename RNGT>
OneBitSCM<T, RNGT>::~OneBitSCM()
{
    this->localData = nullptr;
    this->value = numeric_limits<T>::quiet_NaN();
    // (Base class destructor does)
}

template
class OneBitSCM<float, mt19937>;

template
class OneBitSCM<double, mt19937>;

template
class OneBitSCM<double, mt19937_64>;
