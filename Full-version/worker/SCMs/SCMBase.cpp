#include "SCMBase.h"

#define _USE_MATH_DEFINES
#include <math.h>       // M_PI

#include <algorithm>    // max


// Prepare a bell-shaped weight vector populated based on the normal distribution
template <typename T, typename RNGT>
LocalVector<T> SCMBase<T, RNGT>::PrepareWeightVector(const T w_max, const T sigma, const bool isOneType)
{
    int nw = std::max(this->numRows, this->numCols);
    int n2 = nw / 2;
    if (nw - 2 * n2 == 0)
    {
        nw++;
    }

    auto weight = LocalVector<T>(nw, AllocMode::allRanksNew);
    T *w_data = weight.data;

    int c = (nw - 1) / 2;
    int cutPoint = div_round(nw, 4);

    // Initialise weight vector
    int mu = (nw - 1) / 2;
    T pi = (T)M_PI;
    for (int i = 0; i < nw; i++)
    {
        w_data[i] = exp(-T(0.5) * ((i - mu) * (i - mu) / sigma / sigma)) / (sqrt(2 * pi) * sigma);
    }

    // Weight vector transformation
    for (int i = c + cutPoint + 1; i < nw; i++)
    {
        w_data[i] = 0;
    }
    for (int i = 0; i < c - cutPoint; i++)
    {
        w_data[i] = 0;
    }
    T norm = w_data[c];
    if (isOneType)
    {
        w_data[c] = 0;
        norm = w_data[c + 1];
    }
    for (int i = 0; i < nw; i++)
    {
        w_data[i] *= w_max / norm;
    }

    return weight;
}

template
class SCMBase<float, std::mt19937>;

template
class SCMBase<double, std::mt19937>;

template
class SCMBase<double, std::mt19937_64>;
