#include "DistEnv.h"
#include <math.h>   // floor


namespace DistEnv
{
    int numRanks;
    int myRank;
    int numThreads;

    // Custom MPI types necessary to send/receive objects of types std::mt19937 and std::mt19937_64 in graceful way
    MPI_Datatype MPI_MT19937;
    MPI_Datatype MPI_MT19937_64;
}

int GetRankChunkStartIdx(int totalLength, int rank)
{
    using namespace DistEnv;

    double idx = rank / (double)numRanks * totalLength;
    return (int)floor(idx + 0.5);   // Rounding to the nearest integer
}

int GetRankChunkLength(int totalLength, int rank)
{
    return GetRankChunkStartIdx(totalLength, rank + 1) - GetRankChunkStartIdx(totalLength, rank);
}

int GetRankNumber(int totalLength, int idx)
{
    using namespace DistEnv;

    for (int i = 0; i < numRanks; i++)
    {
        if (idx < GetRankChunkStartIdx(totalLength, i + 1))
        {
            return i;
        }
    }
    return numRanks;
}

int GetThreadChunkStartIdx(int localLength, int thread)
{
    using namespace DistEnv;

    double idx = thread / (double)numThreads * localLength;
    return (int)floor(idx + 0.5);   // Rounding to the nearest integer
}

int div_round(int n, int m)
{
    int d = n / m;
    int rem = n - m * d;
    if (m / 2 <= rem) d = d + 1;
    return d;
}

int div_round(int64_t n, int64_t m)
{
    int64_t d = n / m;
    int64_t rem = n - m * d;
    if (m / 2 <= rem) d = d + 1;
    return (int)d;
}