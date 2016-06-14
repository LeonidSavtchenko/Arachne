#pragma once

#include "SCMBase.h"
#include "GetTypeTagUtils.h"

// The class is a substitute for distributed matrix of synaptic conductance,
// with all elements equal zero.
template <typename T, typename RNGT>
class ZeroSCM : public SCMBase<T, RNGT>
{
public:
    
    // Compute product taking history into scope
    void MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;
    void MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;

    // Default constructor
    ZeroSCM();

    // Custom constructor.
    // Distributed matrix is created given number of rows and columns.
    ZeroSCM(int numRows, int numCols);

    // Destructor
    ~ZeroSCM();
};