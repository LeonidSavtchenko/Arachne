#pragma once

#include <iostream>
#include <sstream>
#include <omp.h>

#include "DistEnv.h"

#define Marker(marker)                              \
    {                                               \
        std::stringstream ss;                       \
        ss << "myRank = " << DistEnv::myRank <<     \
        ", myThread = " << omp_get_thread_num() <<  \
        ", marker = \"" << #marker << "\"\n";       \
        std::cout << ss.str() << std::flush;        \
    }

#define FreezeOnIter(freezeIter)    \
    if (iter == freezeIter)         \
    {                               \
        while (true);               \
    }

// Use it with multiple ranks, but only one thread
#define PrintDistVector(vec)                                                \
    {                                                                       \
        using namespace DistEnv;                                            \
                                                                            \
        for (int rank = 0; rank < numRanks; rank++)                         \
        {                                                                   \
            if (myRank == rank)                                             \
            {                                                               \
                for (int i = 0; i < vec.localLength; i++)                   \
                {                                                           \
                    std::cout << "rank " << myRank << ":\t" <<              \
                        #vec << '[' << i << "] =\t" << vec[i] << std::endl; \
                }                                                           \
            }                                                               \
            MPI_Barrier(MPI_COMM_WORLD);                                    \
        }                                                                   \
        std::cout << std::flush;                                            \
    }
