#include "ZeroSCM.h"
#include "DistEnv.h"

#include <omp.h>
#include <stdint.h> // uint64_t


// Compute product taking history into scope
template <typename T, typename RNGT>
void ZeroSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    y.AssignZeros();
}

// Overload function which computes product taking history into scope
template <typename T, typename RNGT>
void ZeroSCM<T, RNGT>::MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates)
{
    y.AssignZeros();
}

// Default constructor
template <typename T, typename RNGT>
ZeroSCM<T, RNGT>::ZeroSCM()
{
}

// Custom constructor.
// numRows, numCols - number of rows and total number of columns.
template <typename T, typename RNGT>
ZeroSCM<T, RNGT>::ZeroSCM(int numRows, int numCols)
{
    using namespace DistEnv;
    
    this->numRows = numRows;
    this->numCols = numCols;
    this->localNumCols = GetRankChunkLength(numCols, myRank);
    this->mpiTypeTag = GetMpiDataType<T>();
}

// Destructor.
template <typename T, typename RNGT>
ZeroSCM<T, RNGT>::~ZeroSCM()
{
    // (Base class destructor does)
}

template
class ZeroSCM<float, std::mt19937>;

template
class ZeroSCM<double, std::mt19937>;

template
class ZeroSCM<double, std::mt19937_64>;
