#pragma once

#include "SCMBase.h"
#include "Containers/LocalVector.h"
#include "Containers/DistVector.h"
#include "Containers/LocalDenseMatrix.h"
#include "Containers/DistDenseMatrix.h"
#include "SclModel.h"
#include "MathUtils.h"
#include <mpi.h>


template <typename T, typename RNGT>
class SimpleSCM : public SCMBase<T, RNGT>, public DistDenseMatrix<T, RNGT>
{
public:

    // Maximum value of elements
    T max_value;
    
    // Compute product taking history into scope
    void MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;
    void MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;

    // Default constructor
    SimpleSCM();

    // 1st custom constructor
    SimpleSCM(DistDenseMatrix<T, RNGT> &distMatrix);

    // 2nd custom constructor.
    // SCM is created given number of rows and columns.
    // Each rank contains all rows and some number of columns.
    SimpleSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, SclModel sclModel, bool isOneType, const DistVector<int> &seeds);

    // Destructor
    ~SimpleSCM();

private:

    // Internal part of 2nd custom constructor
    void SimpleSCMMultithreaded(const LocalVector<T> &weight, T g_hat, SclModel sclModel, const DistVector<int> &seeds);

};