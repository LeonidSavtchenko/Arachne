#include "DistEnv.h"
#include "LocalVector.h"
#include "DistVector.h"
#include "LocalDenseMatrix.h"
#include "memory.h"
#include "GetTypeTagUtils.h"

#include <mpi.h>

#include <stdint.h> // uint8_t, uint64_t
#include <omp.h>

#include <random>   // mt19937, mt19937_64
#include <iostream>

using namespace std;

// Default constructor
template <typename T>
DistVector<T>::DistVector()
{
    this->localData = nullptr;
    this->localLength = -1;
}

// Custom constructor.
// Given length, create distributed vector.
// The memory is allocated on each rank, but not initialized.
template <typename T>
DistVector<T>::DistVector(int length)
{
    using namespace DistEnv;
    
    this->length = length;
    this->localLength = GetRankChunkLength(length, myRank);
    this->localData = new T[localLength];
}

// Create a deep copy of this vector.
// (The method should be called on all ranks with single thread.)
template <typename T>
DistVector<T> DistVector<T>::Clone() const
{
    // Allocate memory
    auto clone = DistVector<T>(this->length);

    T *inData = this->localData;
    T *outData = clone.localData;
    int len = this->localLength;

    // Copy data by threads
    #pragma omp parallel
    {
        int myThread = omp_get_thread_num();
        int startIdx = GetThreadChunkStartIdx(len, myThread);
        int endIdx = GetThreadChunkStartIdx(len, myThread + 1);
        
        memcpy(outData + startIdx, inData + startIdx, sizeof(T) * (endIdx - startIdx));
    }
    
    return clone;
}

// Assign zeros to all elements of this vector and return a reference to it.
// (The method should be called on all ranks with multiple threads.)
template <typename T>
DistVector<T>& DistVector<T>::AssignZeros()
{
    using namespace DistEnv;
    
    T *data = this->localData;
    int len = this->localLength;
    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(len, myThread);
    int endIdx = GetThreadChunkStartIdx(len, myThread + 1);
    
    // Assign zeros by threads
    memset(data + startIdx, 0, sizeof(T) * (endIdx - startIdx));
    
    return *this;
}

// Populate distributed vector with random numbers,
// update the states of random number generators.
template <typename T>
template <typename RNGT>
DistVector<T>& DistVector<T>::FillRandom(DistributionWrapper &minToMax, DistVector<RNGT> &states)
{
    int myThread = omp_get_thread_num();
    RNGT &myState = states[myThread];

    int yStart = GetThreadChunkStartIdx(localLength, myThread);
    int yEnd = GetThreadChunkStartIdx(localLength, myThread + 1);
    for (int i = yStart; i < yEnd; i++)
    {
        localData[i] = minToMax.Next<T>(myState);
    }
    return *this;
}

template
DistVector<float>& DistVector<float>::FillRandom(DistributionWrapper &minToMax, DistVector<mt19937> &states);

template
DistVector<double>& DistVector<double>::FillRandom(DistributionWrapper &minToMax, DistVector<mt19937> &states);

template
DistVector<double>& DistVector<double>::FillRandom(DistributionWrapper &minToMax, DistVector<mt19937_64> &states);

// Given distributed vector, gather data following the mode specified by onlyMaster flag:
// false - gather into local vectors preallocated on each rank
//         (local vectors with the same data live on each rank after gathering);
// true  - gather only into local vector living on master rank.
template <typename T>
void DistVector<T>::Gather(LocalVector<T> &vector, bool onlyMaster) const
{
    // Barrier before master's solo
    #pragma omp barrier
    
    #pragma omp master
    {
        using namespace DistEnv;
        
        int totalLength = this->length;
        T *data = vector.data;
    
        for (int i = 0; i < numRanks; i++)
        {
            // Determine local chunk length and start index
            int chunkLength = GetRankChunkLength(totalLength, i);
            int startIdx = GetRankChunkStartIdx(totalLength, i);

            if (!onlyMaster)
            {
                // Gather on all ranks.
                // (Broadcast data from each rank to all other ranks.)

                T *ptr;

                if (myRank == i)
                {
                    // Copy own chunk
                    memcpy(data + startIdx, localData, chunkLength * sizeof(T));

                    // Prepare send buffer pointer
                    ptr = localData;
                }
                else
                {
                    // Prepare receive buffer pointer
                    ptr = data + startIdx;
                }
                MPI_Bcast(ptr, chunkLength, this->mpiTypeTag, i, MPI_COMM_WORLD);
            }
            else
            {
                // Gather on master rank only.
                // (Send data from each rank to master rank.)
            
                if (myRank == MASTER_RANK)
                {
                    if (i == 0)
                    {
                        // Copy own chunk
                        memcpy(data, localData, chunkLength * sizeof(T));
                    }
                    else
                    {
                        // Receive data from ith rank
                        MPI_Recv(data + startIdx, chunkLength, this->mpiTypeTag, i, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
                    }
                }
                else if (myRank == i)
                {
                    // Send data to master rank
                    MPI_Send(localData, chunkLength, this->mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD);
                }
            
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
    }
    
    // Barrier after master's solo
    #pragma omp barrier
}

// Given distributed vector, do incremental gathering from all ranks to local vector living on master rank.
//
// Input arguments:
// localCounter - number of elements to be gathered from given rank
//                (the elements with indices 0, 1, ..., localCounter - 1 will be gathered,
//                localCounter must not exceed this->localLength);
// startIdx     - index of the first element in vector to gather to
//                (the first element gathered from the first rank will be placed into position startIdx,
//                the first element gathered from the second rank will be placed into position startIdx + localCounter_for_the_first_rank
//                and so forth).
//                This argument is read only on master rank and updated after gathering.
//
// Left-hand output argument:
// True if success, false if no enough space in the target vector to gather to.
template <typename T>
bool DistVector<T>::Gather(LocalVector<T> &vector, int localCounter, int &startIdx) const
{
    static bool success;
    
    // Barrier before master's solo
    #pragma omp barrier

    #pragma omp master
    {
        using namespace DistEnv;

        T *data = vector.data;
    
        // First of all, check if there is enough space in the target vector to gather to
        
        // Get total number of elements to gather from all ranks
        int totalNumelt = 0;
        MPI_Reduce(&localCounter, &totalNumelt, 1, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);

        // Do check on master rank
        bool enoughSpace = false;
        if (myRank == MASTER_RANK)
        {
            if (startIdx + totalNumelt <= vector.length)
            {
                // It's enough space in the target vector to gather to
                enoughSpace = true;
            }
        }

        // Broadcast the enough space flag to all ranks
        MPI_Bcast(&enoughSpace, 1, MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);
        
        if (!enoughSpace)
        {
            goto Label;
        }
        
        // Do incremental gathering
        for (int i = 0; i < numRanks; i++)
        {
            int counterTag = 2 * i;     // Tag used to send/receive localCounter
            int dataTag = 2 * i + 1;    // Tag used to send/receive data

            if (myRank == MASTER_RANK)
            {
                if (i == 0)
                {
                    // Copy own chunk
                    memcpy(data + startIdx, localData, localCounter * sizeof(T));
                    startIdx += localCounter;
                }
                else
                {
                    // Receive localCounter from ith rank
                    MPI_Recv(&localCounter, 1, MPI_INT, i, counterTag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

                    if (localCounter != 0)
                    {
                        // Receive data from ith rank
                        MPI_Recv(data + startIdx, localCounter, this->mpiTypeTag, i, dataTag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
                        startIdx += localCounter;
                    }
                }
            }
            else if (myRank == i)
            {
                // Send localCounter to master rank
                MPI_Send(&localCounter, 1, MPI_INT, MASTER_RANK, counterTag, MPI_COMM_WORLD);

                if (localCounter != 0)
                {
                    // Send data to master rank
                    MPI_Send(localData, localCounter, this->mpiTypeTag, MASTER_RANK, dataTag, MPI_COMM_WORLD);
                }
            }
            
            MPI_Barrier(MPI_COMM_WORLD);
        }

Label:
        success = enoughSpace;
    }

    // Barrier after master's solo
    #pragma omp barrier

    return success;
}

// Populate entire column of the local matrix living on the master rank with selected elements of this distributed vector.
// The vector "srcIdxs" must be same on all ranks. 
// The number of rows in "dstMatrix" must be equal to the length of "srcIdxs".
template <typename T>
template <typename RNGT>
void DistVector<T>::CopySliceToMatrixColumn(
    /*out*/ LocalDenseMatrix<T, RNGT> &dstMatrix,
    int dstColIdx,
    const LocalVector<int> &srcIdxs) const
{
    // Barrier before master's solo
    #pragma omp barrier
    
    #pragma omp master
    {
        using namespace DistEnv;

        int srcVecTotalLength = this->length;
        int numRows = srcIdxs.length;
        T *srcVecLocalData = this->localData;
        T *dstMatData = dstMatrix.data;
        
        for (int i = 0; i < numRows; i++)
        {
            int srcRankIdx = GetRankNumber(srcVecTotalLength, srcIdxs[i]);
            int localIdx = srcIdxs[i] - GetRankChunkStartIdx(srcVecTotalLength, srcRankIdx);
            
            if (srcRankIdx == MASTER_RANK)
            {
                if (myRank == MASTER_RANK)
                {
                    dstMatData[numRows * dstColIdx + i] = srcVecLocalData[localIdx];
                }
            }
            else
            {
                if (myRank == srcRankIdx)
                {
                    T *ptr = &srcVecLocalData[localIdx];
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
void DistVector<float>::CopySliceToMatrixColumn(LocalDenseMatrix<float, std::mt19937> &dstMatrix, int dstColIdx, const LocalVector<int> &srcRowColIdxs) const;

template
void DistVector<double>::CopySliceToMatrixColumn(LocalDenseMatrix<double, std::mt19937> &dstMatrix, int dstColIdx, const LocalVector<int> &srcRowColIdxs) const;

template
void DistVector<double>::CopySliceToMatrixColumn(LocalDenseMatrix<double, std::mt19937_64> &dstMatrix, int dstColIdx, const LocalVector<int> &srcRowColIdxs) const;


// Destructor
template <typename T>
DistVector<T>::~DistVector()
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
    localLength = -1;
}


template
class DistVector<float>;

template
class DistVector<double>;

template
class DistVector<bool>;

template
class DistVector<uint8_t>;

template
class DistVector<int>;

template
class DistVector<uint32_t>;

template
class DistVector<uint64_t>;

template
class DistVector<mt19937>;

template
class DistVector<mt19937_64>;