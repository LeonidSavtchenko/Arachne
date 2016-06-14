#pragma once

#include <random>

// A wrapper class for std::bernoulli_distribution which provides significantly better performance
// for p = 0 and p = 1 (roughly 8 times for RNGT = std::mt19937_64)
// by the cost of slightly worse performance for other values (roughly 1.15 times for the same RNGT)
template <typename T, typename RNGT>
class BoostedBernoulliDistribution
{
private:
    bool boostedMode;
    bool outValue;
    std::bernoulli_distribution bd;

public:
    BoostedBernoulliDistribution()
    {
    }

    BoostedBernoulliDistribution(T p)
    {
        SetP(p);
    }

    inline bool operator () (RNGT &state)
    {
        if (boostedMode)
        {
            return outValue;
        }
        else
        {
            return bd(state);
        }
    }
    
    inline T GetP()
    {
        return (T)bd.p();
    }

    inline void SetP(T p)
    {
        if (p == 0)
        {
            boostedMode = true;
            outValue = false;
        }
        else if (p == 1)
        {
            boostedMode = true;
            outValue = true;
        }
        else
        {
            boostedMode = false;
            bd = std::bernoulli_distribution((double)p);
        }
    }
};