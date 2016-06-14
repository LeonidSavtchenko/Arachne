#include "GABA.h"
#include "MatFileIO/MatFileIOUtils.h"


template <typename T>
GABA<T>::GABA()
{
    this->num_i = -1;
}

template <typename T>
GABA<T>::GABA(int num_i, int m_steps_prev, int m_steps, T dt)
{
    this->num_i = num_i;
    this->m_steps_prev = m_steps_prev;
    this->m_steps = m_steps;
    this->dt = dt;
    this->dt05 = dt / 2;
}

template <typename T>
void GABA<T>::ReadInputDataAllocateTemporaryArrays(bool continuationMode)
{
    using namespace DistEnv;

    DeltaVGABA = ReadCheckBroadcastScalar<T>("DeltaVGABA");
    GTonicGABA = ReadCheckBroadcastScalar<T>("GTonicGABA");
    VTonicGABA = ReadCheckBroadcastScalar<T>("VTonicGABA");
    AlphaTonic = ReadCheckBroadcastScalar<T>("AlphaTonic");
    
    // Parameters of dynamic tonic current conductance
    enableFreqDelay = false;
    dynamicGTonicGABA = ReadCheckBroadcastScalar<bool>("dynamicGTonicGABA");
    if (dynamicGTonicGABA)
    {
        watchGTonicGABA = ReadCheckBroadcastScalar<bool>("watchGTonicGABA");
        Af = ReadCheckBroadcastScalar<T>("Af");
        Gpump = ReadCheckBroadcastScalar<T>("Gpump");
        GTonicGABAControl = ReadCheckBroadcastScalar<T>("GTonicGABAControl");
        basicFrequency = ReadCheckBroadcastScalar<T>("basicFrequency");

        freq_i = 0;
        if (continuationMode)
        {
            GTonicGABA = ReadCheckBroadcastScalar<T>("GTonicGABA", MatFile::IntermInput);
        }
        if (watchGTonicGABA)
        {
            GTonicGABA_vec = LocalVector<T>(m_steps + 1, AllocMode::onlyMasterMxCalloc);
            if (!continuationMode)
            {
                if (myRank == MASTER_RANK)
                {
                    GTonicGABA_vec[0] = GTonicGABA;
                }
            }
            else
            {
                ReadCheckInVector(GTonicGABA_vec, "GTonicGABA", m_steps_prev + 1, MatFile::Output);
            }
        }
        enableFreqDelay = ReadCheckBroadcastScalar<bool>("enableFreqDelay");
        if (enableFreqDelay)
        {
            freqDelay = ReadCheckBroadcastScalar<int>("freqDelay");
        }
    }

}

template <typename T>
void GABA<T>::WriteOutputData(int num_steps)
{
    // Tonic current conductance
    WriteScalar<bool>(dynamicGTonicGABA, "dynamicGTonicGABA");
    if (dynamicGTonicGABA)
    {
        WriteScalar<bool>(watchGTonicGABA, "watchGTonicGABA");
        if (watchGTonicGABA)
        {
            WriteCutVector<T>(GTonicGABA_vec, "GTonicGABA", num_steps + 1);
        }
    }
    WriteScalar<bool>(enableFreqDelay, "enableFreqDelay");
}

template <typename T>
void GABA<T>::GatherWriteIntermediateData()
{
    // Tonic current conductance
    if (dynamicGTonicGABA)
    {
        WriteScalar<T>(GTonicGABA, "GTonicGABA", MatFile::Intermediate);
    }
}

template <typename T>
void GABA<T>::DoOneStepPart1()
{
    // Update GTonicGABA
    #pragma omp barrier
    #pragma omp master
    {
        if (dynamicGTonicGABA)
        {
            GTonicGABA_old = GTonicGABA;
            T g_t_inc = GTonicGABA_rhp();
            GTonicGABA += dt05 * g_t_inc;
        }
    }
    #pragma omp barrier
}

template <typename T>
void GABA<T>::DoOneStepPart2(int iter)
{
    using namespace DistEnv;

    // Update GTonicGABA
    #pragma omp barrier
    #pragma omp master
    {
        if (dynamicGTonicGABA)
        {
            T g_t_inc = GTonicGABA_rhp();
            GTonicGABA = GTonicGABA_old + dt * g_t_inc;
            if (watchGTonicGABA && myRank == MASTER_RANK)
            {
                GTonicGABA_vec[iter + 1] = GTonicGABA;
            }
        }
    }
    #pragma omp barrier
}


template
class GABA<float>;

template
class GABA<double>;
