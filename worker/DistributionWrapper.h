#pragma once

#include <random>

// Wrapper of class uniform_real_distribution<T> serving for resolving of the following collision:
// 1) The class uniform_real_distribution<T> can be compiled only for T = float or double.
// 2) The class DistVector<T> must be compiled for T = float, double, int, std::mt19937 etc.
// 3) The method DistVector<T>::FillRandom must pass the distribution object of type T.
// The collision is critical for Linux compiler, but not critical for MSVS compiler.
// (The latter gets that the method DistVector<T>::FillRandom is not called anywhere for T = int, std::mt19937 etc.)
class DistributionWrapper
{
private:
    std::uniform_real_distribution<float> *dist_f;
    std::uniform_real_distribution<double> *dist_d;

public:
    template <typename T>
    DistributionWrapper(T lowerBound, T upperBound);

    DistributionWrapper();

    // Inlined method "Next"
    template <typename T>
    inline T Next(std::mt19937 &state) const
    {
        // Neither float, nor double type of probability distribution -- not supported mode
        throw;
    }

    // Inlined method "Next"
    template <typename T>
    inline T Next(std::mt19937_64 &state) const
    {
        // Neither float, nor double type of probability distribution -- not supported mode
        throw;
    }
};


// Specializations of inlined method "Next"
// (moved out of the class body to make Linux compiler happy)

template <>
inline float DistributionWrapper::Next<float>(std::mt19937 &state) const
{
    return (*dist_f)(state);
}

template <>
inline double DistributionWrapper::Next<double>(std::mt19937 &state) const
{
    return (*dist_d)(state);
}

template <>
inline double DistributionWrapper::Next<double>(std::mt19937_64 &state) const
{
    return (*dist_d)(state);
}