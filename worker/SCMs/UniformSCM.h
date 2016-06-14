#pragma once

#include "SCMBase.h"
#include "Containers/DistVector.h"


// The class is a substitute for distributed matrix of synaptic conductance,
// with all elements equal to one constant value.
template <typename T, typename RNGT>
class UniformSCM : public SCMBase<T, RNGT>
{
public:

    // Value, matrix is filled with
    T value;
    
    // Compute product taking history into scope
    void MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;
    void MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;

    // Default constructor
    UniformSCM();

    // Custom constructor.
    // Matrix is created given number of rows, columns and constant value.
    UniformSCM(int numRows, int numCols, LocalVector<T> releaseProb, T value);

    // Destructor
    ~UniformSCM();
};