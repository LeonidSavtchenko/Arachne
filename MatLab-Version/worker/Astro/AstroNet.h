#pragma once

#include "BoostedBernoulliDistribution.h"
#include "Containers/DistVector.h"
#include "Containers/LocalVector.h"
#include "Containers/LocalDenseMatrix.h"

// Astrocyte network

// The main purpose of this class is solving of the following system of 4 ODEs:

//  Ca'[t] = -c1 v2 (Ca[t] - CaER) - (v3 Ca[t] ^ 2) / (k3 ^ 2 + Ca[t] ^ 2) - c1 v1 (ip3[t] / (ip3[t] + d1)) ^ 3 (Ca[t] / (Ca[t] + d5)) ^ 3 q[t] ^ 3 (Ca[t] - CaER)
//   q'[t] = a2 d2 (ip3[t] + d1) / (ip3[t] + d3) (1 - q[t]) - a2 Ca[t] q[t]
// ip3'[t] = (ip3star - ip3[t]) / tip3 + rip3 y[t]
//   y'[t] = -y[t] / tin + tauspike sum_of_delta_functions

template <typename T, typename RNGT>
class AstroNet
{
public:

    // Public methods
    void ReadInputDataAllocateTemporaryArrays(bool continuationMode, std::vector<BoostedBernoulliDistribution<T, RNGT>> &rvg);
    void WriteOutputData(int num_steps);
    void GatherWriteIntermediateData();
    void DoOneStepPart1(DistVector<int> &lastSpikes_e, int lastNumSpikes_e);
    void DoOneStepPart2(int iter, DistVector<int> &lastSpikes_e, int lastNumSpikes_e);
    void CorrectReleaseProbability(std::vector<BoostedBernoulliDistribution<T, RNGT>> &rvg, int iter);
    
    // Default constructor
    AstroNet();
    
    // Custom constructor
    AstroNet(int num_a, int m_steps_prev, int m_steps, T dt, T timeScale);

private:

    void ReadTransitMxArrays();
    void WriteTransitMxArrays();

    // Number of astrocytes (it equals num_e)
    int num_a;
    
    // Number of steps
    int m_steps_prev, m_steps, caColormapPeriodIter;

    // Boolean scalars
    bool gatherCaColormap;

    // Float-point scalars
    T v1, v2, v3, d1, d2, d3, d5;
    T c1, a2, k3, ip3star, tip3, rip3, gs;
    T tau_spike, t_in, p_basic, CaBA, Ca_ER;
    T dt, dt05, timeScale;
    
    // Vectors
    LocalVector<T> Ca_local, ip3_local, q_local, y_local;
    DistVector<T> Ca, ip3, q, y;
    DistVector<T> Ca_tmp, ip3_tmp, q_tmp, y_tmp;
    
    // Parameters for visualization
    int watchedAstroNum;
    LocalVector<int> watchedAstroIdx;
    LocalDenseMatrix<T, RNGT> watchedAstroCa, watchedAstroY, watchedProb;
    LocalDenseMatrix<T, RNGT> CaColormap; 
    
    // Temporary data
    LocalVector<T> Ca_buf;
    T minCa, plCa;

    // Declarations of inlined methods defined outside the class
    // (no "inline" keyword needed in these declarations)
    T Ca_rhp(const T &Ca, const T &ip3, const T &q);
    T ip3_rhp(const T &ip3, const T &y);
    T q_rhp(const T &Ca, const T &ip3, const T &q);

    // Input MAT file verbatims
    mxArray* watchedAstroIdxOneBased;
};

// Definitions of inlined methods
#include "Ca_rhp.h"
#include "ip3_rhp.h"
#include "q_rhp.h"