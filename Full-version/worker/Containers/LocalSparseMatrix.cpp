#include "LocalSparseMatrix.h"
#include "DistEnv.h"
#include "GetTypeTagUtils.h"

#include <iostream>

#include <mpi.h>

using namespace std;


// Default constructor
template <typename T, typename RNGT>
LocalSparseMatrix<T, RNGT>::LocalSparseMatrix()
{
    sr = nullptr;
    ir = nullptr;
    jc = nullptr;
    numElem = -1;
}

// Custom constructor. Called only on master thread of the master rank.
template <typename T, typename RNGT>
LocalSparseMatrix<T, RNGT>::LocalSparseMatrix(mxArray *data)
{
    numElem = static_cast<int>(mxGetNzmax(data));
    this->numRows = static_cast<int>(mxGetM(data));
    this->numCols = static_cast<int>(mxGetN(data));

    double *pSr = mxGetPr(data);
    size_t *pIr = mxGetIr(data);
    size_t *pJc = mxGetJc(data);

    // Cast indices from 64-bit integers to 32-bit integers for performance.
    // If gamma simulator is running under single-precision arithmetics,
    // then cast matrix data from double to float.
    // (Matlab does not support single-precision sparse matrix.)

    sr = (T*)pSr;
    ir = (int*)pIr;
    jc = (int*)pJc;

    if (sizeof(T) == sizeof(float))
    {
        for (int idx = 0; idx < numElem; idx++)
        {
            sr[idx] = static_cast<float>(pSr[idx]);
            ir[idx] = static_cast<int>(pIr[idx]);
        }
    }
    else
    {
        for (int idx = 0; idx < numElem; idx++)
        {
            ir[idx] = static_cast<int>(pIr[idx]);
        }
    }

    for (int idx = 0; idx < this->numCols + 1; idx++)
    {
        jc[idx] = static_cast<int>(pJc[idx]);
    }

    this->mpiTypeTag = GetMpiDataType<T>();
}

// Scatter from rank 0 to all ranks so that each rank contained some number of columns
template <typename T, typename RNGT>
DistSparseMatrix<T, RNGT> LocalSparseMatrix<T, RNGT>::Scatter() const
{
    using namespace DistEnv;

    // Broadcast local matrix height and width from rank 0 to all other ranks
    int totalNumRows = 0;
    int totalNumCols = 0;
    int totalNumElem = 0;
    if (myRank == MASTER_RANK)
    {
        totalNumRows = this->numRows;
        totalNumCols = this->numCols;
        totalNumElem = numElem;
    }
    MPI_Bcast(&totalNumRows, 1, MPI_UNSIGNED, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(&totalNumCols, 1, MPI_UNSIGNED, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(&totalNumElem, 1, MPI_UNSIGNED, MASTER_RANK, MPI_COMM_WORLD);
    
    // Allocate memory on each rank
    auto distSparseMatrix = DistSparseMatrix<T, RNGT>(totalNumElem, totalNumRows, totalNumCols);

    if (myRank == MASTER_RANK)
    {
        // Determine number of elements to send and start index
        int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
        int rankNumCols = GetRankChunkLength(totalNumCols, myRank);
        int rankNumElem = jc[startIdx + rankNumCols] - jc[startIdx];
        
        // Copy own chunk
        memcpy(distSparseMatrix.sr, sr, rankNumElem * sizeof(T));
        memcpy(distSparseMatrix.ir, ir, rankNumElem * sizeof(int));
        memcpy(distSparseMatrix.jc, jc, (rankNumCols + 1) * sizeof(int));
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    // Scatter data from rank 0 to ranks 1, 2, ..., numRanks - 1
    for (int i = 1; i < numRanks; i++)
    {
        if (myRank == MASTER_RANK)
        {
            // Determine number of elements to send and start index
            int startIdx = GetRankChunkStartIdx(totalNumCols, i);
            int rankNumCols = GetRankChunkLength(totalNumCols, i);
            int rankNumElems = jc[startIdx + rankNumCols] - jc[startIdx];

            // Send local matrix chunk
            MPI_Send(&jc[startIdx], rankNumCols + 1, GetMpiDataType<int>(), i, i, MPI_COMM_WORLD);
            MPI_Send(&sr[jc[startIdx]], rankNumElems, this->mpiTypeTag, i, i, MPI_COMM_WORLD);
            MPI_Send(&ir[jc[startIdx]], rankNumElems, GetMpiDataType<int>(), i, i, MPI_COMM_WORLD);
        }
        else if (myRank == i)
        {
            int length = distSparseMatrix.localNumCols;

            MPI_Recv(distSparseMatrix.jc, length + 1, GetMpiDataType<int>(), MASTER_RANK, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            for (int k = distSparseMatrix.localNumCols; k >= 0; k--)
            {
                distSparseMatrix.jc[k] -= distSparseMatrix.jc[0];
            }
            int numElem = 0;
            if (length > 0)
            {
                numElem = distSparseMatrix.jc[distSparseMatrix.localNumCols];
            }
            MPI_Recv(distSparseMatrix.sr, numElem, this->mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Recv(distSparseMatrix.ir, numElem, GetMpiDataType<int>(), MASTER_RANK, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    return distSparseMatrix;
}

// Destructor
template <typename T, typename RNGT>
LocalSparseMatrix<T, RNGT>::~LocalSparseMatrix()
{
    // Remark: We do not deallocate memory in all container destructors by two reasons.
    // 1) It's necessary to create shallow copies for containers in some cases
    //    (e.g. a copy with shifted data pointer),
    //    but it's not necessary to deallocate memory when the copy is destructed.
    // 2) All new containers (not shallow copies) are created in gamma simulator only before 1st iteration
    //    and used at time of whole simulation session.
    //    The memory is deallocated as a whole by OS when MPI process dies.

    // delete[] sr;
    // delete[] ir;
    // delete[] jc;
    sr = nullptr;
    ir = nullptr;
    jc = nullptr;
    numElem = -1;
}

template
class LocalSparseMatrix<float, mt19937>;

template
class LocalSparseMatrix<double, mt19937>;

template
class LocalSparseMatrix<double, mt19937_64>;

