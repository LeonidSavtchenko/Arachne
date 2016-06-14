#include <stdint.h>
#include <random>

#include "MathUtils.h"

// Check if float x is not a number
bool isNaN(float x)
{
    uint32_t mask = 0x7fc00000;
    uint32_t bits = *(uint32_t*)&x;
    return (bits & mask) == mask;
}

// Check if double x is not a number
bool isNaN(double x)
{
    uint64_t mask = 0x7ff8000000000000;
    uint64_t bits = *(uint64_t*)&x;
    return (bits & mask) == mask;
}

// Check if x has not a number
template <typename T, typename RNGT>
bool hasNaN(LocalDenseMatrix<T, RNGT> x)
{
    for (int i = 0; i < x.numRows * x.numCols; i++)
    {
        if (isNaN(x.data[i]))
        {
            return true;
        }
    }
    return false;
}

template
bool hasNaN<float, std::mt19937>(LocalDenseMatrix<float, std::mt19937> x);

template
bool hasNaN<double, std::mt19937>(LocalDenseMatrix<double, std::mt19937> x);

template
bool hasNaN<double, std::mt19937_64>(LocalDenseMatrix<double, std::mt19937_64> x);