#include "DistSparseMatrix.h"
#include "DistEnv.h"
#include "GetTypeTagUtils.h"

#include <omp.h>

#include <vector>

using namespace std;


// Default constructor
template <typename T, typename RNGT>
DistSparseMatrix<T, RNGT>::DistSparseMatrix()
{
    this->sr = nullptr;
    this->ir = nullptr;
    this->jc = nullptr;
}

// First custom constructor.
// Given sr, ir and jc arrays, create distributed sparse matrix.
// Each rank contains some number of columns.
template <typename T, typename RNGT>
DistSparseMatrix<T, RNGT>::DistSparseMatrix(int numElem, int numRows, int numCols)
{
    using namespace DistEnv;
    
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->sr = new T[numElem];
    this->ir = new int[numElem];
    this->jc = new int[this->localNumCols + 1];
    this->mpiTypeTag = GetMpiDataType<T>();
}

// Destructor
template <typename T, typename RNGT>
DistSparseMatrix<T, RNGT>::~DistSparseMatrix()
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

    // (Base class destructor does)
}


template
class DistSparseMatrix<float, mt19937>;

template
class DistSparseMatrix<double, mt19937>;

template
class DistSparseMatrix<double, mt19937_64>;

