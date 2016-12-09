#pragma once

#include "Containers/LocalVector.h"
#include "Containers/DistVector.h"
#include "DistributionWrapper.h"
#include "SimpleSCM.h"
#include <omp.h>


// The class is a substitute for distributed matrix of synaptic conductance.
// All matrix elements are not stored in physical memory.
// Each element is generated only in the place when it's used.
template <typename T, typename RNGT>
class InPlaceSCM : public SCMBase<T, RNGT>
{

private:

    typedef typename RNGT::result_type result_type;

    T value;
    result_type boundary;
    DistVector<int> seeds;
    DistVector<RNGT> states;
    
    SclModel sclModel;
    LocalVector<T> weight;
    LocalVector<result_type> int_weight;

public:
    
    // Compute product taking history into scope
    void MatVecSubstitute(DistVector<T> &y, const LocalDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;
    void MatVecSubstitute(DistVector<T> &y, const DistDenseMatrix<T, RNGT> &X, int delta, int currentIdx, DistVector<RNGT> &releaseGenStates) override;

    // Default constructor
    InPlaceSCM();

    // Custom constructor
    // (the seeds vector is copied)
    InPlaceSCM(int numRows, int numCols, T w_max, T g_hat, T sigma, LocalVector<T> releaseProb, SclModel sclModel, bool isOneType, const DistVector<int> seeds);
    
    // Destructor
    ~InPlaceSCM();

private:

    // Methods

    // Re-initialize internal random number generators
    // (must be called each time before start of matrix generation)
    inline void Reseed()
    {
        int myThread = omp_get_thread_num();
        int mySeed = seeds[myThread];
        states[myThread].seed(mySeed);
    }

    // Generate the next element of the matrix
    inline result_type NextElem()
    {
        int myThread = omp_get_thread_num();
        RNGT &myState = states[myThread];
        result_type randomValue = myState();
        return randomValue;
    }
};