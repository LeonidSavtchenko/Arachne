#include "InPlaceSCM.h"
#include "DistEnv.h"
#include "GetTypeTagUtils.h"

#include <limits>   // quiet_NaN

using namespace std;
using namespace DistEnv;


// Compute product taking history into scope
template <typename T, typename RNGT>
void InPlaceSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    using namespace DistEnv;
    
    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round(n, 4);
    int totalNumCols = this->numCols;

    int64_t nLong = n;
    int64_t totalNumColsLong = totalNumCols;

    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
    SclModel sclModel = this->sclModel;
    T g_hat = this->value;
        
    const T *xData = X.data;
    T *yData = y.localData;
    
    // Re-initialize internal random number generators
    this->Reseed();
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(m, myThread);
    int yEndIdx = GetThreadChunkStartIdx(m, myThread + 1);
    
    int nw = this->weight.length;
    int c = (nw - 1) / 2;
    T *w_data = this->weight.data;
    result_type *int_data = this->int_weight.data;
    T elem;
    
    for (int i = yStartIdx; i < yEndIdx; i++)
    {
        yData[i] = 0;
    }
    
    // Loop by y (destination cell index)
    for (int pre = -cutPoint; pre <= cutPoint; pre++)
    {
        // Get index of the past iteration we take into scope now
        int delay = 1 + abs(pre);
        int pastIdx = currentIdx - delay * delta;
        if (pastIdx < 0)
        {
            pastIdx += max_delay;
        }
        
        for (int i = yStartIdx; i < yEndIdx; i++)
        {
            // Source cell index
            int j0 = div_round((startIdx + i) * nLong, totalNumColsLong);
            int j = this->GetRowIdx(j0, pre);
            
            // Generate current element of matrix
            int index = c + div_round(((i + startIdx) * (nLong - 1) - j * (totalNumColsLong - 1)) * (nw - 1), (nLong - 1) * (totalNumColsLong - 1));
            while (index < 0) index += nw;
            while (index > nw - 1) index -= nw;
            switch (sclModel)
            {
                case SclModel::BSS: 
                    elem = w_data[index];
                    break;
                case SclModel::BSD:
                    elem = (this->NextElem() < int_data[index]) ? g_hat : 0;
                    break;
            }
            
            bool s = this->rvg[j](releaseGenStates[myThread]);
            if (s)
            {
                yData[i] += elem * xData[pastIdx * n + j];
            }
        }
    }
}

// Overload function which computes product taking history into scope
template <typename T, typename RNGT>
void InPlaceSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    using namespace DistEnv;

    int m = y.localLength;
    int n = this->numRows;
    int max_delay = X.numCols;
    int cutPoint = div_round((int)n, 4);
    int totalNumCols = this->numCols;

    int64_t nLong = n;
    int64_t totalNumColsLong = totalNumCols;

    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
    SclModel sclModel = this->sclModel;
    T g_hat = this->value;
        
    const T *xData = X.localData;
    T *yData = y.localData;
    
    T *xColData = this->x_tmp.data;
    
    // Re-initialize internal random number generators
    this->Reseed();
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(m, myThread);
    int yEndIdx = GetThreadChunkStartIdx(m, myThread + 1);
    
    int nw = this->weight.length;
    int c = (nw - 1) / 2;
    T *w_data = this->weight.data;
    result_type *int_data = this->int_weight.data;
    T elem;
    
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
            int j0 = div_round((startIdx + i) * nLong, totalNumColsLong);
            int j = this->GetRowIdx(j0, pre);
           
            // Generate current element of matrix
            int index = c + div_round(((i + startIdx) * (nLong - 1) - j * (totalNumColsLong - 1)) * (nw - 1), (nLong - 1) * (totalNumColsLong - 1));
            while (index < 0) index += nw;
            while (index > nw - 1) index -= nw;
            switch (sclModel)
            {
                case SclModel::BSS: 
                    elem = w_data[index];
                    break;
                case SclModel::BSD:
                    elem = (this->NextElem() < int_data[index]) ? g_hat : 0;
                    break;
            }
            
            bool s = this->rvg[j](releaseGenStates[myThread]);
            if (s)
            {
                yData[i] += elem * xColData[j];
            }
        }
    }
}

// Default constructor
template <typename T, typename RNGT>
InPlaceSCM<T, RNGT>::InPlaceSCM()
{
    this->value = numeric_limits<T>::quiet_NaN();
    this->boundary = 0;
}

// Custom constructor.
// numRows, numCols - number of rows and total number of columns,
// g_hat, p         - the parameters used when an element of the matrix is generated,
// seeds            - random number generator seeds for each thread of each process
//                    (the seeds vector is copied).
template <typename T, typename RNGT>
InPlaceSCM<T, RNGT>::InPlaceSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, SclModel sclModel, bool isOneType, const DistVector<int> seeds)
{
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->value = g_hat;
    this->sclModel = sclModel;
    this->seeds = seeds;
    this->states = DistVector<RNGT>(numRanks * numThreads);
    this->boundary = states[0].max();   // Remark: Linux compiler does not allow syntax RNGT::max()
    this->x_tmp = LocalVector<T>(numRows, AllocMode::allRanksNew);
    this->mpiTypeTag = GetMpiDataType<T>();
    this->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
    for (int j = 0; j < numRows; j++)
    {
        this->rvg[j] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[j]);
    }
    
    weight = this->PrepareWeightVector(w_max, sigma, isOneType);
    int nw = weight.length;

    if (sclModel == SclModel::BSD)
    {
        int_weight = LocalVector<result_type>(nw, AllocMode::allRanksNew);
        for (int i = 0; i < nw; i++)
        {
            if (weight[i] < 1)
            {
                int_weight[i] = (result_type)(weight[i] * boundary + T(0.5));
            }
            else
            {
                int_weight[i] = boundary;
            }
        }
    }
}

// Destructor
template <typename T, typename RNGT>
InPlaceSCM<T, RNGT>::~InPlaceSCM()
{
    this->value = numeric_limits<T>::quiet_NaN();
    this->boundary = 0;
    // (Base class destructor does)
}

template
class InPlaceSCM<float, mt19937>;

template
class InPlaceSCM<double, mt19937>;

template
class InPlaceSCM<double, mt19937_64>;
