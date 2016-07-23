#pragma once

#include "Containers/LocalVector.h"


// Gamma-Aminobutyric Acid mechanisms (DeltaVGABA, GTonicGABA, VTonicGABA)
template <typename T>
class GABA
{
    
public:
    
    // GABA parameters
    T DeltaVGABA;
    T GTonicGABA;
    T VTonicGABA;
    T AlphaTonic;
    
    // Variables for dynamic tonic current conductance
    bool dynamicGTonicGABA, enableFreqDelay;
    int freqDelay;
    T freq_i;

    // Constructors
    GABA();
    GABA(int num_i, int m_steps_prev, int m_steps, T dt);

    // I/O staff
    void ReadInputDataAllocateTemporaryArrays(bool continuationMode);
    void WriteOutputData(int num_steps);
    void GatherWriteIntermediateData();

    // GTonicGABA ODE integration
    void DoOneStepPart1();
    void DoOneStepPart2(int iter);
    
private:
    
    // Variables for dynamic tonic current conductance
    bool watchGTonicGABA;
    T Af, Gpump, GTonicGABAControl;
    T GTonicGABA_old;
    T basicFrequency;
    LocalVector<T> GTonicGABA_vec;
    
    // General simulation parameters
    int num_i;
    int m_steps_prev, m_steps;
    T dt, dt05;

    // GTonicGABA ODE right-hand part
    inline T GTonicGABA_rhp()
    {
        return num_i * Af * (freq_i + basicFrequency) - Gpump * (GTonicGABA - GTonicGABAControl);
    }
    
};