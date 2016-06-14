#pragma once

#include "SCMBase.h"
#include "Containers/LocalVector.h"
#include "Containers/DistVector.h"
#include "Containers/DistDenseMatrix.h"
#include "Containers/DistSparseMatrix.h"

#include <matrix.h>
#include <mpi.h>

// The class that represents a matrix of synaptic conductance with sparse structure.
// Contains only nonzero elements and their indexes.
template <typename T, typename RNGT>
class SparseSCM : public SCMBase<T, RNGT>, public DistSparseMatrix<T, RNGT>
{
public:

    // Methods
    
    // Compute product taking history into scope
    void MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;
    void MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;

    // Default constructor
    SparseSCM();

    // 1st custom constructor
    SparseSCM(DistSparseMatrix<T, RNGT> &distSparseMatrix);

    // 2nd custom constructor
    SparseSCM(int numElem, int numRows, int numCols);

    // 3rd custom constructor
    SparseSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, bool isOneType, const DistVector<int> &seeds);

    // Destructor
    ~SparseSCM();
};