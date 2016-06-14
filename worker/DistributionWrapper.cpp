#include "DistributionWrapper.h"

#include <stdint.h> // uint64_t

using namespace std;


template <typename T>
DistributionWrapper::DistributionWrapper(T lowerBound, T upperBound)
{
    // Neither float, nor double type of probability distribution -- not supported mode
    throw;
}

template <>
DistributionWrapper::DistributionWrapper(float lowerBound, float upperBound)
{
    static auto urd = std::uniform_real_distribution<float>(lowerBound, upperBound);
    dist_f = &urd;
    dist_d = nullptr;
}

template <>
DistributionWrapper::DistributionWrapper(double lowerBound, double upperBound)
{
    static auto urd = uniform_real_distribution<double>(lowerBound, upperBound);
    dist_f = nullptr;
    dist_d = &urd;
}

DistributionWrapper::DistributionWrapper()
{
    dist_f = nullptr;
    dist_d = nullptr;
}