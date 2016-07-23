#pragma once

#include "Containers/LocalDenseMatrix.h"
#include "SCMs/SimpleSCM.h"
#include "MatFileIO/MatFileIOUtils.h"

template <typename T, typename RNGT>
class BaseStdpMechanism
{
protected:
    static LocalDenseMatrix<T, RNGT> stdpCommonParams;

    int stdpPeriodIter;
    bool windowedSTDP;
    T s;
    T t_windowedSTDP;
    int colIdx;

private: 
    virtual T AtomicCorrection(const T &W, const T &t1, const T &t2, const T &preFreq, const T &postFreq, T t_new) = 0;
  
    virtual void ReadInputDataAllocateTemporaryArrays();

protected:

    // Keep it in sync with the function of the same name in host/UTILITY_PlotStdpModels.m
    inline T EpsilonSmoothing(T x, T T_t)
    {
        if (abs(x) >= T_t)
        {
            return 0;
        }
        else
        {
            T x_sq = x * x;
            return exp(s * x_sq / (x_sq - T_t * T_t));
        }
    }

public:
    // Hebbian correction
    void HebbianCorrection(
        SimpleSCM<T, RNGT>* scm,
        const LocalVector<T> &times1,
        const LocalVector<int> &indexes1,
        int num1,
        const LocalVector<T> freq1, 
        const LocalVector<T> &times2,
        const LocalVector<int> &indexes2,
        int num2,
        const DistVector<T> freq2,
        LocalDenseMatrix<T, RNGT> &copy_col, T t_new);

    virtual ~BaseStdpMechanism()
    {
    }

protected:
    // Custom constructor
    BaseStdpMechanism(T dt, int colIdx)
    {
        t_windowedSTDP = T(stdpPeriodIter) * dt;
        this->colIdx = colIdx;

        ReadInputDataAllocateTemporaryArrays();
    }
};