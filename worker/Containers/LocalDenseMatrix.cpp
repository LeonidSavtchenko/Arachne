#include "LocalDenseMatrix.h"
#include "SCMs/SimpleSCM.h"
#include "GetTypeTagUtils.h"
#include "DistEnv.h"

#include <memory.h> // memcpy
#include <stdint.h> // uint64_t
#include <iostream>

using namespace std;


// Default constructor
template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT>::LocalDenseMatrix()
{
    this->data = nullptr;
}

// Custom constructor
template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT>::LocalDenseMatrix(T *data, int numRows, int numCols)
{
    this->data = data;
    this->numRows = numRows;
    this->numCols = numCols;
    this->mpiTypeTag = GetMpiDataType<T>();
}

// Custom constructor 2: Allocate memory on this rank with new T[]
template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT>::LocalDenseMatrix(int numRows, int numCols)
{
    this->data = new T[numRows * numCols];
    this->numRows = numRows;
    this->numCols = numCols;
    this->mpiTypeTag = GetMpiDataType<T>();
}

// Custom constructor 3: Allocate memory on master rank or all ranks with new T[] or mxCalloc
template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT>::LocalDenseMatrix(int numRows, int numCols, AllocModeMatrix allocMode)
{
    using namespace DistEnv;

    switch (allocMode)
    {
        case AllocModeMatrix::MasterMxCallocOtherNew:
            {
                if (myRank == MASTER_RANK)
                {
                    this->data = (T*)mxCalloc(numRows * numCols, sizeof(T));   // mxCalloc initializes data to zero
                    this->numRows = numRows;
                    this->numCols = numCols;
                }
                else
                {
                    this->data = new T[numRows * numCols];
                    this->numRows = numRows;
                    this->numCols = numCols;
                }
                this->mpiTypeTag = GetMpiDataType<T>();
                break;
            }
        case AllocModeMatrix::MasterMxCalloc:
            {
                if (myRank == MASTER_RANK)
                {
                    this->data = (T*)mxCalloc(numRows * numCols, sizeof(T));   // mxCalloc initializes data to zero
                    this->numRows = numRows;
                    this->numCols = numCols;
                }
                else
                {
                    this->data = nullptr;
                    this->numRows = -1;
                    this->numCols = -1;
                }
                this->mpiTypeTag = GetMpiDataType<T>();
                break;
            }
        default:
            {
                cout << "\n>>>>> Bad allocMode specified for LocalDenseMatrix<T, RNGT> constructor.\n" << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
    }
}

// Fill matrix with zeros
template <typename T, typename RNGT>
void LocalDenseMatrix<T, RNGT>::FillZeros()
{
    int length = this->numRows * this->numCols * sizeof(T);
    void *data = this->data;
    memset(data, 0, length);
}

// Scatter from rank 0 to all ranks so that each rank contained some number of columns
template <typename T, typename RNGT>
DistDenseMatrix<T, RNGT> LocalDenseMatrix<T, RNGT>::Scatter() const
{
    using namespace DistEnv;

    // Broadcast local matrix height and width from rank 0 to all other ranks
    int totalNumRows = this->numRows;
    int totalNumCols = this->numCols;
    MPI_Bcast(&totalNumRows, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(&totalNumCols, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    // Allocate memory on each rank
    auto distMatrix = DistDenseMatrix<T, RNGT>(totalNumRows, totalNumCols);

    // Get pointer to local data
    T *localData = distMatrix.localData;

    int localNumCols = distMatrix.localNumCols;
    int localNumElem = totalNumRows * localNumCols;
    
    if (myRank == MASTER_RANK)
    {
        // Copy own chunk
        memcpy(localData, data, localNumElem * sizeof(T));
    }

    // Scatter data from rank 0 to ranks 1, 2, ..., numRanks - 1
    for (int i = 1; i < numRanks; i++)
    {
        if (myRank == MASTER_RANK)
        {
            // Determine number of elements to send and start index
            int numElem = totalNumRows * GetRankChunkLength(totalNumCols, i);
            int startIdx = totalNumRows * GetRankChunkStartIdx(totalNumCols, i);

            // Send local matrix chunk
            MPI_Send(&data[startIdx], numElem, this->mpiTypeTag, i, i, MPI_COMM_WORLD);
        }
        else if (myRank == i)
        {
            MPI_Recv(localData, localNumElem, this->mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    return distMatrix;
}

// Fill one column of the local matrix (matrices) with data from the distributed vector.
// If onlyMaster == true, then only matrix on master rank receives the data.
// If onlyMaster == false, then matrices on all ranks receive the data (the matrices are supposed to be clones).
template <typename T, typename RNGT>
void LocalDenseMatrix<T, RNGT>::FillColFromVector(const DistVector<T> &vector, int colIdx, bool onlyMaster)
{
    using namespace DistEnv;

    // Barrier before master's solo
    #pragma omp barrier
    
    #pragma omp master
    {
        T *colStart = this->data + colIdx * this->numRows;

        if (onlyMaster)
        {
            if (myRank == MASTER_RANK)
            {
                // Copy own chunk
                memcpy(colStart, vector.localData, vector.localLength * sizeof(T));
            }

            for (int i = 1; i < numRanks; i++)
            {
                // Send data from all ranks to master rank
                if (myRank == i)
                {
                    MPI_Send(vector.localData, vector.localLength, this->mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD);
                }
                else if (myRank == MASTER_RANK)
                {
                    // Compute the first copied row index and expected chunk length
                    int rowIdx = GetRankChunkStartIdx(this->numRows, i);
                    int length = GetRankChunkStartIdx(this->numRows, i + 1) - rowIdx;

                    MPI_Recv(colStart + rowIdx, length, this->mpiTypeTag, i, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        else
        {
            for (int i = 0; i < numRanks; i++)
            {
                // Gather the column on all ranks.
                // (Broadcast data from each rank to all other ranks.)

                // Compute the first copied row index
                int rowIdx = GetRankChunkStartIdx(this->numRows, i);

                T *ptr;     // Send or receive buffer pointer
                int length; // Local chunk length

                if (myRank == i)
                {
                    // Sender rank

                    // Prepare send buffer pointer and local chunk length
                    ptr = vector.localData;
                    length = vector.localLength;

                    // Copy own chunk
                    memcpy(colStart + rowIdx, ptr, length * sizeof(T));
                }
                else
                {
                    // Receiver rank

                    // Compute the local chunk length
                    length = GetRankChunkStartIdx(this->numRows, i + 1) - rowIdx;

                    // Prepare receive buffer pointer
                    ptr = colStart + rowIdx;
                }
                MPI_Bcast(ptr, length, this->mpiTypeTag, i, MPI_COMM_WORLD);
            }
        }
    }

    // Barrier after master's solo
    #pragma omp barrier
}

// Destructor
template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT>::~LocalDenseMatrix()
{
    // Remark: We do not deallocate memory in all container destructors by two reasons.
    // 1) It's necessary to create shallow copies for containers in some cases
    //    (e.g. a copy with shifted data pointer),
    //    but it's not necessary to deallocate memory when the copy is destructed.
    // 2) All new containers (not shallow copies) are created in gamma simulator only before 1st iteration
    //    and used at time of whole simulation session.
    //    The memory is deallocated as a whole by OS when MPI process dies.

    // delete[] data;
    data = nullptr;
}


template
class LocalDenseMatrix<float, std::mt19937>;

template
class LocalDenseMatrix<double, std::mt19937>;

template
class LocalDenseMatrix<double, std::mt19937_64>;

template
class LocalDenseMatrix<bool, std::mt19937>;

template
class LocalDenseMatrix<bool, std::mt19937_64>;

template
class LocalDenseMatrix<uint64_t, std::mt19937>;

template
class LocalDenseMatrix<uint64_t, std::mt19937_64>;

template
class LocalDenseMatrix<int, std::mt19937>;

template
class LocalDenseMatrix<int, std::mt19937_64>;
