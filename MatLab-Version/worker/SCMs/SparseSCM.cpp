#include "SparseSCM.h"
#include "DistEnv.h"
#include "GetTypeTagUtils.h"

#include <omp.h>

#include <vector>

using namespace std;


// Compute product taking history into scope
template <typename T, typename RNGT>
void SparseSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
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
    
    const T *xData = X.data;
    const T *matData = this->sr;
    T *yData = y.localData;
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(m, myThread);
    int yEndIdx = GetThreadChunkStartIdx(m, myThread + 1);
    
    // Loop by y (destination cell index)
    for (int i = yStartIdx; i < yEndIdx; i++)
    {
        yData[i] = 0;
        int j0 = div_round((startIdx + i) * nLong, totalNumColsLong);
        int jc1 = this->jc[i];
        int jc2 = this->jc[i + 1];
        for (int idx = jc1; idx < jc2; idx++)
        {
            int j = this->ir[idx];
            int pre = j - j0;
            while (pre < -cutPoint)
                pre += n;
            while (pre > cutPoint)
                pre -= n;
            
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
                yData[i] += matData[j] * xData[pastIdx * n + j];
            }
        }
    }
}

// Overload function which computes product taking history into scope
template <typename T, typename RNGT>
void SparseSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
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
    
    const T *xData = X.localData;
    const T *matData = this->sr;
    T *yData = y.localData;
    
    T *xColData = this->x_tmp.data;
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(m, myThread);
    int yEndIdx = GetThreadChunkStartIdx(m, myThread + 1);
    
    for (int i = yStartIdx; i < yEndIdx; i++) y[i] = 0;
    
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

            bool s = this->rvg[j](releaseGenStates[myThread]);
            if (s)
            {
                // Take into account influence of cell j to cell i
                int jc1 = this->jc[i];
                int jc2 = this->jc[i + 1];
                for (int idx = jc1; idx < jc2; idx++)
                {
                    if (j == this->ir[idx])
                    {
                        yData[i] += matData[j] * xColData[j];
                    }
                }
            }
        }
    }
}

// Default constructor
template <typename T, typename RNGT>
SparseSCM<T, RNGT>::SparseSCM()
{
    this->sr = nullptr;
    this->ir = nullptr;
    this->jc = nullptr;
}

// Constructor that takes a base class object
template <typename T, typename RNGT>
SparseSCM<T, RNGT>::SparseSCM(DistSparseMatrix<T, RNGT> &distSparseMatrix)
{
    this->sr = distSparseMatrix.sr;
    this->ir = distSparseMatrix.ir;
    this->jc = distSparseMatrix.jc;
    this->numRows = distSparseMatrix.numRows;
    this->numCols = distSparseMatrix.numCols;
    this->localNumCols = distSparseMatrix.localNumCols;
    this->mpiTypeTag = GetMpiDataType<T>();
    this->x_tmp = LocalVector<T>(this->numRows, AllocMode::allRanksNew);
}

// First custom constructor.
// Given sr, ir and jc arrays, create distributed sparse matrix.
// Each rank contains some number of columns.
template <typename T, typename RNGT>
SparseSCM<T, RNGT>::SparseSCM(int numElem, int numRows, int numCols)
{
    using namespace DistEnv;
    
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->sr = new T[numElem];
    this->ir = new int[numElem];
    this->jc = new int[GetRankChunkLength(numCols, myRank) + 1];
    this->x_tmp = LocalVector<T>(numRows, AllocMode::allRanksNew);
    this->mpiTypeTag = GetMpiDataType<T>();
}

// Second custom constructor.
// The memory is allocated on each rank, then synaptic conductance matrix is populated based on input arguments.
template <typename T, typename RNGT>
SparseSCM<T, RNGT>::SparseSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, bool isOneType, const DistVector<int> &seeds)
{
    using namespace DistEnv;
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
    for (int j = 0; j < numRows; j++)
    {
        this->rvg[j] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[j]);
    }
    
    int startIdx = GetRankChunkStartIdx(numCols, myRank);
    int mySeed = seeds[myRank];
    auto myState = RNGT((unsigned long)mySeed);
    
    LocalVector<T> weight = this->PrepareWeightVector(w_max, sigma, isOneType);
    const T *w_data = weight.data;

    auto maxValue = myState.max();  // Remark: Linux compiler does not allow syntax RNGT::max()
    typedef typename RNGT::result_type result_type;
    T boundary = T(maxValue);
    
    // Triplet arrays for local data of distributed sparse matrix
    auto *vSr = new vector<T>();
    auto *vIr = new vector<int>();
    this->jc = new int[this->localNumCols + 1];
    
    T value = g_hat;
    
    int m = numRows;
    int totalNumCols = numCols;
    int64_t mLong = m;
    int64_t totalNumColsLong = totalNumCols;
    int cutPoint = div_round(m, 4);
    
    int nw = max(numRows, numCols);
    int n2 = nw / 2;
    if (nw - 2 * n2 == 0) nw++;
    int c = (nw - 1) / 2;

    result_type randomValue;
    int index;
    int idx = 0;
    for (int i = 0; i < this->localNumCols; i++)
    {
        this->jc[i] = idx;
        int j0 = div_round((startIdx + i) * mLong, totalNumColsLong);
        for (int pre = -cutPoint; pre <= cutPoint; pre++)
        {
            // Source cell index
            int j = this->GetRowIdx(j0, pre);
        
            randomValue = myState();
            index = c + div_round(((i + startIdx) * (mLong - 1) - j * (totalNumColsLong - 1)) * (nw - 1), (mLong - 1) * (totalNumColsLong - 1));
            while (index < 0) index += nw;
            while (index > nw - 1) index -= nw;
            if (randomValue / boundary < w_data[index])
            {
                vSr->push_back(value);
                vIr->push_back(j);
                idx++;
            }
        }
    }
    this->jc[this->localNumCols] = idx;

    // Bind data to this SparseSCM
    this->sr = vSr->data();
    this->ir = vIr->data();
    
    this->x_tmp = LocalVector<T>(numRows, AllocMode::allRanksNew);
    this->mpiTypeTag = GetMpiDataType<T>();
}

// Destructor
template <typename T, typename RNGT>
SparseSCM<T, RNGT>::~SparseSCM()
{
    // (Base class destructor does)
}


template
class SparseSCM<float, mt19937>;

template
class SparseSCM<double, mt19937>;

template
class SparseSCM<double, mt19937_64>;

