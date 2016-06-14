#pragma once

#include "BaseStdpMechanism.h"

template <typename T, typename RNGT>
class AnalyticStdpMechanism : public BaseStdpMechanism<T, RNGT>
{
private: 
    static LocalDenseMatrix<T, RNGT> stdpParams;

    // Atomic correction.
    // Keep it in sync with the function of the same name in host/UTILITY_PlotStdpModels.m
    inline T AtomicCorrection(const T &W, const T &t1, const T &t2, const T &preFreq, const T &postFreq, T t_new) override
    {
        if (W == 0)
        {
            return 0;
        }
        
        const T *stdpData = stdpParams.data + this->colIdx * (stdpParams.numRows);

        const T &Aplus      = stdpData[0];
        const T &S          = stdpData[1];
        const T &epsilon    = stdpData[2];
        const T &Aminus     = stdpData[3];
        const T &tau2       = stdpData[4];
        const T &S2         = stdpData[5];
        const T &AC         = stdpData[6];
        const T &TimeCosExp = stdpData[7];
        const T &TimeCos    = stdpData[8];
        const T &AS         = stdpData[9];
        const T &TimeSinExp = stdpData[10];
        const T &TimeAbs    = stdpData[11];
        const T &Shift      = stdpData[12];
        const T &SinAmpl    = stdpData[13];
        const T &SinPeriod  = stdpData[14];
        const T &damper     = stdpData[15];
        const T &maxAbsdW   = stdpData[16];

        const T *stdpCommonData = this->stdpCommonParams.data + this->colIdx * (this->stdpCommonParams.numRows);

        const T &preFreqFactor   = stdpCommonData[0];
        const T &postFreqFactor  = stdpCommonData[1];
        const T &freqTermDivisor = stdpCommonData[2];
        const T &mainTermDivisor = stdpCommonData[3];

        // Compute the first term
        T term1 = preFreq * preFreqFactor - postFreq * postFreqFactor;

        // Compute the second term

        T deltat = t1 - t2;
        T delta = deltat * tau2;

        T term2 = 0;
        if (Aplus != 0)
        {
            term2 += Aplus * pow(T(1) - T(1) / delta, S);
        }
        if (epsilon != 0 && Aminus != 0)
        {
            term2 += epsilon * Aminus * pow(T(1) - T(1) / delta, S2);
        }
        if (AC != 0)
        {
            term2 += AC * exp(TimeCosExp * abs(delta)) * T(100) * cos(TimeCos * delta);
        }
        if (AS != 0 && SinAmpl != 0)
        {
            term2 -= AS * exp(TimeSinExp * abs(TimeAbs * delta + Shift)) * SinAmpl * sin(SinPeriod * (delta + Shift));
        }

        if (isNaN(term2))
        {
            // Trap for cases NaN = Inf * 0 and NaN = Inf - Inf
            return 0;
        }

        if (this->windowedSTDP)
        {
            term2 *= this->EpsilonSmoothing(t_new - t1, this->t_windowedSTDP);
            term2 *= this->EpsilonSmoothing(t_new - t2, this->t_windowedSTDP);
        }

        // Compute the factor
        T factor = term1 / freqTermDivisor + term2 / mainTermDivisor;

        T dW = W * factor / damper;
        
        if (dW > maxAbsdW)
        {
            return maxAbsdW;
        }
        else if (dW < -maxAbsdW)
        {
            return -maxAbsdW;
        }
        else
        {
            return dW;
        }
    }

    void ReadInputDataAllocateTemporaryArrays() override
    {
        int stdpParamsNum = ReadCheckBroadcastScalar<int>("stdpParamsNum");
        if (stdpParams.data == nullptr)
        {
            stdpParams = ReadCheckBroadcastMatrix<T, RNGT>("stdpParams", stdpParamsNum, 4);
        }
    }

public:
    // Custom constructor
    AnalyticStdpMechanism(T dt, int colIdx) :
        BaseStdpMechanism<T, RNGT>(dt, colIdx)
    {
        ReadInputDataAllocateTemporaryArrays();
    }
};


