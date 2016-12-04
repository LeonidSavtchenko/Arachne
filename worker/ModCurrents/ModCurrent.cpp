#include "ModCurrent.h"

// Each CPP-file below contains partial implementation of ModCurrent class
#include "MCurReadAllocateWrite.cpp"
#include "MCurDoOneStepPart1.cpp"
#include "MCurDoOneStepPart2.cpp"

// Default constructor
template <typename T, typename RNGT>
ModCurrent<T, RNGT>::ModCurrent()
{
}

// Custom constructor
template <typename T, typename RNGT>
ModCurrent<T, RNGT>::ModCurrent(char suffix, T dt, bool continuationMode, bool enable, int num, const DistVector<T> &v, int m_steps_prev, int m_steps)
{
    if (suffix == 'e')
    {
        p_AllModCurrents = std::make_shared<AllModCurrents_e<T>>(num);
    }
    else if(suffix == 'i')
    {
        p_AllModCurrents = std::make_shared<AllModCurrents_i<T>>(num);
    }
    else
    {
        p_AllModCurrents = nullptr;
    }

    this->suffix = suffix;
    this->dt = dt;
    this->dt05 = dt / 2;
   
    this->m_steps_prev = m_steps_prev;
    this->m_steps = m_steps;

    ReadInputDataAllocateTemporaryArrays(continuationMode, enable, num, v);
}

template
class ModCurrent<float, std::mt19937>;

template
class ModCurrent<double, std::mt19937>;

template
class ModCurrent<double, std::mt19937_64>;