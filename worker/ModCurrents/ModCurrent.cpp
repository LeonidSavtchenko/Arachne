#include "ModCurrent.h"

// Each CPP-file below contains partial implementation of ExtraCurrent class
#include "MCurReadAllocateWrite.cpp"
#include "MCurDoOneStepPart1.cpp"
#include "MCurDoOneStepPart2.cpp"

// Default constructor
template <typename T>
ModCurrent<T>::ModCurrent()
{
}

// Custom constructor
template <typename T>
ModCurrent<T>::ModCurrent(char suffix, T dt, bool continuationMode, bool enable, int num, const DistVector<T> &v, int m_steps_prev, int m_steps)
{
    this->suffix = suffix;
    this->dt = dt;
    this->dt05 = dt / 2;
   
    this->m_steps_prev = m_steps_prev;
    this->m_steps = m_steps;

    ReadInputDataAllocateTemporaryArrays(continuationMode, enable, num, v);
}

template
class ModCurrent<float>;

template
class ModCurrent<double>;
