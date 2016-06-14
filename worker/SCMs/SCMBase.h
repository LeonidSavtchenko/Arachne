#pragma once

#include "BoostedBernoulliDistribution.h"
#include "Containers/DistVector.h"
#include "Containers/LocalDenseMatrix.h"
#include "Containers/DistMatrix.h"

#include <mpi.h>    // MPI_Datatype

// Abstract base class for all Synaptic Conductance Matrix classes:
// SimpleSCM, SparseSCM,
// ZeroSCM, UniformSCM, 
// OneBitSCM, InPlaceSCM.
template <typename T, typename RNGT>
class SCMBase : public virtual DistMatrix<T>
{
public:

    // Random value generators used to provide the space-inhomogeneous release probability mechanism
    std::vector<BoostedBernoulliDistribution<T, RNGT>> rvg;

    // Pure virtual method that computes product taking history into scope
    virtual void MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) = 0;
    virtual void MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) = 0;
    
    inline int GetRowIdx(int rowIdx, int shift)
    {
        rowIdx += shift;
        rowIdx %= this->numRows;
        if (rowIdx >= 0)
        {
            return rowIdx;
        }
        else
        {
            return rowIdx + this->numRows;
        }
    }
    
    // Prepare a bell-shaped weight vector populated based on the normal distribution
    LocalVector<T> PrepareWeightVector(const T w_max, const T sigma, const bool isOneType);

    // Virtual destructor
    virtual ~SCMBase()
    {
    }
};