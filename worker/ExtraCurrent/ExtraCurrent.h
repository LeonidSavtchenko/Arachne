#pragma once

#include "Containers/DistVector.h"
#include "MatFileIO/MatFileIOUtils.h"

template <typename T, typename RNGT>
class ExtraCurrent 
{
public:

    // Constructors

    ExtraCurrent();

    ExtraCurrent(char suffix, T dt, bool continuationMode, bool enable, int num, const DistVector<T> &v, int m_steps_prev, int m_steps);

    // Public methods

    void GatherWriteIntermediateData();
    void WriteOutputData(int num_steps);

    void DoOneStepPart1(const DistVector<T> &v);
    void DoOneStepPart2(int iter, const DistVector<T> &v, const DistVector<T> &v_tmp);

private:
    void ReadInputDataAllocateTemporaryArrays(bool continuationMode, bool enable, int num, const DistVector<T> &v);
    void ReadTransitMxArrays();
    void WriteTransitMxArrays();

public:
    // The current itself
    DistVector<T> I;

private:
    // "e" or "i"
    std::string suffix;

    // Declarations of inlined methods defined outside the class
    // (no "inline" keyword needed in these declarations)
    T a_rhp(T a, T v);
    T b_rhp(T b, T v);
    T ComputeExtraCurrent(T a, T b, T v);

    // Integer scalars
    int p, q;
    
    // Floating-point scalars
    T dt, dt05;

    T g;
    T v_r;
    
    T v_a, s_a, t_a;
    T v_b, s_b, t_b;

    // Distributed vector of activation variables 
    DistVector<T> a;

    // Distributed vector of inactivation variables 
    DistVector<T> b;

    // Local vectors 
    LocalVector<T> a_local, b_local;

    // Temporary data
    DistVector<T> a_tmp;
    DistVector<T> b_tmp;

    // Parameters for visualization
    int watchedExtraCurrentNum;
    LocalVector<int> watchedExtraCurrentIdx;
    LocalDenseMatrix<T, RNGT> watchedExtraCurrentI;

    // Number of steps
    int m_steps_prev, m_steps;

    // Input MAT file verbatims
    mxArray* watchedExtraCurrentIdxOneBased;
};

// Definitions of inlined methods
#include "a_rhp.h"
#include "b_rhp.h"
#include "ComputeExtraCurrent.h"
