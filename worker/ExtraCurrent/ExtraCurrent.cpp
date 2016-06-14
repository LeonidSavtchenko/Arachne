#include "ExtraCurrent.h"

// Each CPP-file below contains partial implementation of ExtraCurrent class
#include "XCurReadAllocateWrite.cpp"
#include "XCurDoOneStepPart1.cpp"
#include "XCurDoOneStepPart2.cpp"

// Default constructor
template <typename T, typename RNGT>
ExtraCurrent<T, RNGT>::ExtraCurrent()
{
}

// Custom constructor
template <typename T, typename RNGT>
ExtraCurrent<T, RNGT>::ExtraCurrent(char suffix, T dt, bool continuationMode, bool enable, int num, const DistVector<T> &v, int m_steps_prev, int m_steps)
{
    this->suffix = suffix;
    this->dt = dt;
    this->dt05 = dt / 2;
   
    this->m_steps_prev = m_steps_prev;
    this->m_steps = m_steps;

    ReadInputDataAllocateTemporaryArrays(continuationMode, enable, num, v);
}

template
class ExtraCurrent<float, std::mt19937>;

template
class ExtraCurrent<double, std::mt19937>;

template
class ExtraCurrent<double, std::mt19937_64>;