#include <stdint.h>  // int64_t
#include <mpi.h>     // MPI_Datatype
#include <algorithm> // max

#include "Debug.h"


#define MASTER_RANK 0


namespace DistEnv
{
    extern int numRanks;
    extern int myRank;
    extern int numThreads;

    // Custom MPI types necessary to send/receive objects of types std::mt19937 and std::mt19937_64 in graceful way
    extern MPI_Datatype MPI_MT19937;
    extern MPI_Datatype MPI_MT19937_64;
}

int GetRankChunkStartIdx(int totalLength, int rank);
int GetRankChunkLength(int totalLength, int rank);

int GetRankNumber(int totalLength, int idx);

int GetThreadChunkStartIdx(int localLength, int thread);

int div_round(int n, int m);
int div_round(int64_t n, int64_t m);
