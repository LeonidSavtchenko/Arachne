#include "SimpleSCM.h"
#include "DistEnv.h"
#include "GetTypeTagUtils.h"

#include "GammaSimulator.h"

#include <omp.h>
#include <stdint.h> // uint64_t
#include <iostream>


using namespace std;


// Compute product taking history into scope
template <typename T, typename RNGT>
void SimpleSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    using namespace DistEnv;
    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round(n, 4);
    int totalNumCols = this->numCols;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);

    int64_t nLong = n;
    int64_t totalNumColsLong = totalNumCols;
        
    const T *matData = this->localData;
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
                // Get index of the past iteration we take into scope now
                int delay = 1 + abs(pre);
                int pastIdx = currentIdx - delay * delta;
                if (pastIdx < 0)
                {
                    pastIdx += max_delay;
                }
                // Take into account influence of cell j to cell i
                yData[i] += matData[i * n + j] * xData[pastIdx * n + j];
            }
        }
    }
}

// Overload function which computes product taking history into scope
template <typename T, typename RNGT>
void SimpleSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    using namespace DistEnv;

    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round(n, 4);
    int totalNumCols = this->numCols;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);

    int64_t nLong = n;
    int64_t totalNumColsLong = totalNumCols;
        
    const T *matData = this->localData;
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
                yData[i] += matData[i * n + j] * xColData[j];
            }
        }
    }
}

// Default constructor
template <typename T, typename RNGT>
SimpleSCM<T, RNGT>::SimpleSCM()
{
    this->localData = nullptr;
}

// Constructor that takes a base class object
template <typename T, typename RNGT>
SimpleSCM<T, RNGT>::SimpleSCM(DistDenseMatrix<T, RNGT> &distMatrix)
{
    this->localData = distMatrix.localData;
    this->numRows = distMatrix.numRows;
    this->numCols = distMatrix.numCols;
    this->localNumCols = distMatrix.localNumCols;
    this->mpiTypeTag = GetMpiDataType<T>();
    this->x_tmp = LocalVector<T>(this->numRows, AllocMode::allRanksNew);
}

// Custom constructor.
// The memory is allocated on each rank, then synaptic conductance matrix is populated based on input arguments.
template <typename T, typename RNGT>
SimpleSCM<T, RNGT>::SimpleSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, SclModel sclModel, bool isOneType, const DistVector<int> &seeds)
{
    using namespace DistEnv;

    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->localData = new T[numRows * this->localNumCols];
    this->mpiTypeTag = GetMpiDataType<T>();
    this->x_tmp = LocalVector<T>(numRows, AllocMode::allRanksNew);
    this->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
    for (int i = 0; i < numRows; i++)
    {
        this->rvg[i] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[i]);
    }
    
    LocalVector<T> weight = this->PrepareWeightVector(w_max, sigma, isOneType);
    
    // Maximum value of matrix elements
    switch (sclModel)
    {
        case SclModel::BSS: 
            this->max_value = 2 * w_max;
            break;
        case SclModel::BSD:
            this->max_value = 2 * g_hat;
            break;
    }
    
    #pragma omp parallel
    {
        SimpleSCMMultithreaded(weight, g_hat, sclModel, seeds);
    }
}

// Internal part of 2nd custom constructor
template <typename T, typename RNGT>
void SimpleSCM<T, RNGT>::SimpleSCMMultithreaded(const LocalVector<T> &weight, T g_hat, SclModel sclModel, const DistVector<int> &seeds)
{
    using namespace DistEnv;
    
    int m = this->numRows;
    int n = this->localNumCols;
    int totalNumCols = this->numCols;
    T *data = this->localData;
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
    memset(data + m * yStartIdx, 0, m * (yEndIdx - yStartIdx) * sizeof(T));

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
            switch (sclModel)
            {
                case SclModel::BSS: 
                    data[j * m + i] = w_data[index];
                    break;
                case SclModel::BSD:
                    result_type randomValue = myState();
                    data[j * m + i] = (randomValue / boundary < w_data[index]) ? g_hat : 0;
                    break;
            }
        }
    }
}

// Destructor
template <typename T, typename RNGT>
SimpleSCM<T, RNGT>::~SimpleSCM()
{
    // (Base class destructor does)
}


template
class SimpleSCM<float, std::mt19937>;

template
class SimpleSCM<double, std::mt19937>;

template
class SimpleSCM<double, std::mt19937_64>;
