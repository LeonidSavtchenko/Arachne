#pragma once

#include "Containers/LocalVector.h"
#include "Containers/DistVector.h"
#include "Containers/DistDenseMatrix.h"
#include "SCMBase.h"

#include <stdint.h> // uint64_t

// The class that represents a matrix of synaptic conductance with each element
// stored in one bit of memory.
template <typename T, typename RNGT>
class OneBitSCM : public SCMBase<T, RNGT>
{
public:

    // Pointer to local chunk of the data stored on this rank.
    // Each value of type uint64_t represents 64 adjacent elements of the matrix living in the same row.
    uint64_t *localData;

    // The value that corresponds to nonzero bit of the matrix
    T value;

private:

    // The matrix of size numRows by numCols bits
    DistDenseMatrix<uint64_t, RNGT> matrix;

public:
    
    // Compute product taking history into scope
    void MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;
    void MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;

    // Default constructor
    OneBitSCM();

    // Custom constructor
    OneBitSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, bool isOneType, const DistVector<int> &seeds);

    // Destructor
    ~OneBitSCM();

private:

    // Internal part of the custom constructor
    void OneBitSCMMultithreaded(const LocalVector<T> &weight, T g_hat, const DistVector<int> &seeds);
};