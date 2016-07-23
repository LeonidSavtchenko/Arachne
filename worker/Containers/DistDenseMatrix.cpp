#include "DistDenseMatrix.h"
#include "DistEnv.h"
#include "GetTypeTagUtils.h"

#include <omp.h>
#include <stdint.h> // uint64_t

#include <iostream>

using namespace std;


// Default constructor
template <typename T, typename RNGT>
DistDenseMatrix<T, RNGT>::DistDenseMatrix()
{
    this->localData = nullptr;
}

// Custom constructor.
// The memory is allocated on each rank, but not initialized.
template <typename T, typename RNGT>
DistDenseMatrix<T, RNGT>::DistDenseMatrix(int numRows, int numCols)
{
    using namespace DistEnv;
    
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->localData = new T[numRows * this->localNumCols];
    this->mpiTypeTag = GetMpiDataType<T>();
    this->x_tmp = LocalVector<T>(numRows, AllocMode::allRanksNew);
}

// Fill matrix with zeros
template <typename T, typename RNGT>
void DistDenseMatrix<T, RNGT>::FillZeros()
{
    int length = this->numRows * this->localNumCols * sizeof(T);
    void *data = this->localData;
    memset(data, 0, length);
}

// Fill one column of the matrix with data from distributed vector
template <typename T, typename RNGT>
void DistDenseMatrix<T, RNGT>::FillColFromVector(const DistVector<T> &vector, int colIdx, bool placeholder)
{
    using namespace DistEnv;
    
    int n = this->numRows;
    int m = this->numCols;
    int num = GetRankNumber(m, colIdx);
    vector.Gather(this->x_tmp);
    T *vector_data = this->x_tmp.data;
    
    if (myRank == num)
    {
        int startIdx = GetRankChunkStartIdx(m, num);
        T *data = this->localData + (colIdx - startIdx) * n;
        int myThread = omp_get_thread_num();
        int yStartIdx = GetThreadChunkStartIdx(n, myThread);
        int yEndIdx = GetThreadChunkStartIdx(n, myThread + 1);
        for (int i = yStartIdx; i < yEndIdx; i++)
        {
            data[i] = vector_data[i];
        }
    }
}

// Fill one row of the matrix with data from distributed vector
template <typename T, typename RNGT>
void DistDenseMatrix<T, RNGT>::FillRowFromVector(const DistVector<T> &vector, int rowIdx)
{
    int myThread = omp_get_thread_num();
    int rankLength = this->localNumCols;
    int startIdx = GetThreadChunkStartIdx(rankLength, myThread);
    int endIdx = GetThreadChunkStartIdx(rankLength, myThread + 1);
    T *data = this->localData + rowIdx;

    for (int i = startIdx; i < endIdx; ++i)
    {
        data[i * this->numRows] = vector[i];
    }

}

// Given distributed matrix, gather data into local matrix living on master rank.
template <typename T, typename RNGT>
void DistDenseMatrix<T, RNGT>::Gather(LocalDenseMatrix<T, RNGT> &matrix)
{
    // Barrier before master's solo
    #pragma omp barrier
    
    #pragma omp master
    {
        using namespace DistEnv;
        
        int totalNumCols = this->numCols;
        int numRows = this->numRows;
        T *data = matrix.data;
    
        for (int i = 0; i < numRanks; i++)
        {
            // Determine local chunk length and start index
            int length = GetRankChunkLength(totalNumCols, i) * numRows;
            int startIdx = GetRankChunkStartIdx(totalNumCols, i) * numRows;

            // Gather on master rank only.
            // (Send data from each rank to master rank.)
            
            if (myRank == MASTER_RANK)
            {
                if (i == 0)
                {
                    // Copy own chunk
                    memcpy(data, localData, length * sizeof(T));
                }
                else
                {
                    // Receive data from ith rank
                    MPI_Recv(data + startIdx, length, this->mpiTypeTag, i, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
                }
            }
            else if (myRank == i)
            {
                // Send data to master rank
                MPI_Send(localData, length, this->mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD);
            }
            
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    
    // Barrier after master's solo
    #pragma omp barrier
}

// Populate entire column of the local matrix living on the master rank with selected elements of this distributed matrix.
// The matrix "srcRowColIdxs" must be same on all ranks.
// The number of rows in "dstMatrix" must be equal to the number of rows in "srcRowColIdxs".
template <typename T, typename RNGT>
void DistDenseMatrix<T, RNGT>::CopySliceToMatrixColumn(
    /*out*/ LocalDenseMatrix<T, RNGT> &dstMatrix,
    int dstColIdx,
    const LocalDenseMatrix<int, RNGT> &srcRowColIdxs    /* The matrix of size numRows-by-2 */
    ) const
{
    // Barrier before master's solo
    #pragma omp barrier
    
    #pragma omp master
    {
        using namespace DistEnv;

        int srcMatNumRows = this->numRows;
        int srcMatTotalNumCols = this->numCols;
        int numRows = srcRowColIdxs.numRows;
        T *srcMatLocalData = this->localData;
        T *dstMatData = dstMatrix.data;
        int *srcRowColIdxsData = srcRowColIdxs.data;

        for (int i = 0; i < numRows; i++)
        {
            int srcRowIdx = srcRowColIdxsData[i];
            int srcColIdx = srcRowColIdxsData[numRows + i];

            int srcRankIdx = GetRankNumber(srcMatTotalNumCols, srcColIdx);
            int localSrcColIdx = srcColIdx - GetRankChunkStartIdx(srcMatTotalNumCols, srcRankIdx);
            
            if (srcRankIdx == MASTER_RANK)
            {
                if (myRank == MASTER_RANK)
                {
                    dstMatData[numRows * dstColIdx + i] = srcMatLocalData[srcMatNumRows * localSrcColIdx + srcRowIdx];
                }
            }
            else
            {
                if (myRank == srcRankIdx)
                {
                    T *ptr = &srcMatLocalData[srcMatNumRows * localSrcColIdx + srcRowIdx];
                    MPI_Send(ptr, 1, this->mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD);
                }
                else if (myRank == MASTER_RANK)
                {
                    T *ptr = &dstMatData[numRows * dstColIdx + i];
                    MPI_Recv(ptr, 1, this->mpiTypeTag, srcRankIdx, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
                }

                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
    }

    // Barrier after master's solo
    #pragma omp barrier
}

template
void DistDenseMatrix<float, std::mt19937>::CopySliceToMatrixColumn(LocalDenseMatrix<float, std::mt19937> &dstMatrix, int dstColIdx, const LocalDenseMatrix<int, std::mt19937> &srcRowColIdxs) const;

template
void DistDenseMatrix<double, std::mt19937>::CopySliceToMatrixColumn(LocalDenseMatrix<double, std::mt19937> &dstMatrix, int dstColIdx, const LocalDenseMatrix<int, std::mt19937> &srcRowColIdxs) const;

template
void DistDenseMatrix<double, std::mt19937_64>::CopySliceToMatrixColumn(LocalDenseMatrix<double, std::mt19937_64> &dstMatrix, int dstColIdx, const LocalDenseMatrix<int, std::mt19937_64> &srcRowColIdxs) const;


// Destructor
template <typename T, typename RNGT>
DistDenseMatrix<T, RNGT>::~DistDenseMatrix()
{
    // Remark: We do not deallocate memory in all container destructors by two reasons.
    // 1) It's necessary to create shallow copies for containers in some cases
    //    (e.g. a copy with shifted data pointer),
    //    but it's not necessary to deallocate memory when the copy is destructed.
    // 2) All new containers (not shallow copies) are created in gamma simulator only before 1st iteration
    //    and used at time of whole simulation session.
    //    The memory is deallocated as a whole by OS when MPI process dies.

    // delete[] localData;
    localData = nullptr;
}


template
class DistDenseMatrix<float, std::mt19937>;

template
class DistDenseMatrix<double, std::mt19937>;

template
class DistDenseMatrix<double, std::mt19937_64>;

template
class DistDenseMatrix<bool, mt19937>;

template
class DistDenseMatrix<bool, mt19937_64>;

template
class DistDenseMatrix<uint64_t, mt19937>;

template
class DistDenseMatrix<uint64_t, mt19937_64>;

template
class DistDenseMatrix<int, mt19937>;

template
class DistDenseMatrix<int, mt19937_64>;
