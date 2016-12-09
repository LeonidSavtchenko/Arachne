#pragma once

#include "BaseStdpMechanism.h"
#include <math.h>   // floor

template <typename T, typename RNGT>
class TabulatedStdpMechanism : public BaseStdpMechanism<T, RNGT>
{
private:
    static bool staticVectorsAreRead;
    static LocalVector<T>  stdp_factor;
    static LocalVector<int> size_factor;
    static LocalVector<T> t_start_factor;
    static LocalVector<T> t_end_factor;
    
    int factorNum;

    inline T LinearInterpolation(T deltat)
    {
        T deltat_start = t_start_factor[this->colIdx];
        T deltat_end = t_end_factor[this->colIdx];
        
        double diff1 = deltat - deltat_start;
        double diff2 = deltat - deltat_end;

        if (diff1 < 0 || diff2 > 0)
        {
            return 0;
        }
        
        int size = size_factor[this->colIdx];

        int startIdx = 0;
        int tmpColIdx = this->colIdx;

        while (tmpColIdx > 0)
        {
            tmpColIdx--;
            startIdx += size_factor[tmpColIdx];
        }

        T h = (deltat_end - deltat_start) / (size - 1);

        int currIdx = (int)floor((deltat - deltat_start) / h);

        int i = currIdx + startIdx;

        T resInterpolation = stdp_factor[i] + (stdp_factor[i + 1] - stdp_factor[i]) * ((deltat - deltat_start) / h - currIdx);

        return resInterpolation;
    }

    inline T AtomicCorrection(const T &W, const T &t1, const T &t2, const T &preFreq, const T &postFreq, T t_new) override
    {
        if (W == 0)
        {
            return 0;
        }

        const T *stdpCommonData = this->stdpCommonParams.data + this->colIdx * (this->stdpCommonParams.numRows);

        const T &preFreqFactor   = stdpCommonData[0];
        const T &postFreqFactor  = stdpCommonData[1];
        const T &freqTermDivisor = stdpCommonData[2];
        const T &mainTermDivisor = stdpCommonData[3];

        // Compute the first term
        T term1 = preFreq * preFreqFactor - postFreq * postFreqFactor;

        // Compute the second term
        T deltat = t1 - t2;
        T term2 = LinearInterpolation(deltat);
        if (this->windowedSTDP)
        {
            term2 *= this->EpsilonSmoothing(t_new - t1, this->t_windowedSTDP);
            term2 *= this->EpsilonSmoothing(t_new - t2, this->t_windowedSTDP);
        }

        // Compute the factor
        T factor = term1 / freqTermDivisor + term2 / mainTermDivisor;

        T dW = W * factor;

        return dW;
    }

    void ReadInputDataAllocateTemporaryArrays() override
    {
        factorNum = ReadCheckBroadcastScalar<int>("factorNum");

        if (!staticVectorsAreRead)
        {
            stdp_factor = ReadCheckBroadcastVector<T>("stdp_factor", factorNum);
            size_factor = ReadCheckBroadcastVector<int>("size_factor", 4);
            t_start_factor = ReadCheckBroadcastVector<T>("t_start_factor", 4);
            t_end_factor = ReadCheckBroadcastVector<T>("t_end_factor", 4);
            staticVectorsAreRead = true;
        }
    }

public:
    // Custom constructor
    TabulatedStdpMechanism(T dt, int colIdx) :
        BaseStdpMechanism<T, RNGT>(dt, colIdx)
    {
        ReadInputDataAllocateTemporaryArrays();
    }
};

