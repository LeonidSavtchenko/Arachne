#include "AstroNet.h"

// Each CPP-file below contains partial implementation of AstroNet class
#include "AstroReadAllocateWrite.cpp"
#include "AstroDoOneStepPart1.cpp"
#include "AstroDoOneStepPart2.cpp"
#include "AstroCorrectReleaseProbability.cpp"


// Default constructor
template <typename T, typename RNGT>
AstroNet<T, RNGT>::AstroNet()
{
    this->num_a = -1;
}

// Custom constructor
template <typename T, typename RNGT>
AstroNet<T, RNGT>::AstroNet(int num_a, int m_steps_prev, int m_steps, T dt, T timeScale)
{
    this->num_a = num_a;
    this->m_steps_prev = m_steps_prev;
    this->m_steps = m_steps;
    this->dt = dt / timeScale;
    this->timeScale = timeScale;
}


template
class AstroNet<float, std::mt19937>;

template
class AstroNet<double, std::mt19937>;

template
class AstroNet<double, std::mt19937_64>;