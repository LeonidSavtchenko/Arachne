// Do not complain that fopen is used instead of more secure version fopen_s.
// Justification -- Linux compiler does not support fopen_s.
#define _CRT_SECURE_NO_WARNINGS

#include <engine.h>
#include <mat.h>
#include <matrix.h>

#include "DistEnv.h"
#include "GetTypeTagUtils.h"
#include "MatFileIO/MatFileIOUtils.h"
#include "GammaSimulator.h"

#include <stdio.h>      // fopen, printf
#include <mpi.h>
#include <omp.h>

#include <iostream>     // cout, flush
#include <cstdio>       // fclose, remove
#include <random>       // mt19937, mt19937_64


using namespace std;


template <typename T, typename RNGT>
void main_templated
(
    bool continuationMode,
    bool setIter,
    int m_steps,
    T m_time,
    bool imageMode,
    bool recallMode
);


// Expected input arguments in working mode:
// argc = 5 or 6;
// argv[0] - this executable file name ("gs.exe"),
// argv[1] - the number of threads per process,
// argv[2] - the flag (0/1) indicating whether previous simulation session should be continued,
// argv[3] - the number of iterations or the time interval (in ms) to compute in this simulation session,
// argv[4] - the flag (0/1) indicating whether the time interval (0) or the number of iterations (1) is specified,
// argv[5] - (optional) the flag (0/1) indicating whether to disable STDP for this simulation session (presence of the argument indicates that image mode is on).
//
// Expected input arguments in testing mode
// (we verify whether the number of available cluster nodes is sufficient):
// argc = 1;
// argv[0] - this executable file name ("gs.exe").
int main(int argc, char* argv[])
{
    using namespace DistEnv;
    
    // Check if it is testing mode
    if (argc == 1)
    {
        int one = 1;
        MPI_Init(&one, &argv);
        MPI_Finalize();
        return 0;
    }
    
    // Sanity check
    if (argc != 5 && argc != 6)
    {
        printf("\n>>>>> Five or six input arguments are expected while %i arguments are provided.\n", argc);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    
    int one = 1;
    MPI_Init(&one, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // Read and set number of OMP threads per MPI process
    numThreads = atoi(argv[1]);
    omp_set_num_threads(numThreads);
    
    // !! TODO: Check that numThreads is parsed successfully and does not exceed omp_get_num_procs()

    // Print message saying that HPC kernel is started with specified number of processes and threads per process
    if (myRank == MASTER_RANK)
    {
        printf("HPC kernel started with np = %i, nt = %i.\n", numRanks, numThreads);
        cout << flush;
    }

    // Read the flag indicating whether previous simulation session should be continued
    bool continuationMode = (atoi(argv[2]) != 0);
    
    // Read the flag indicating whether the period or the number of iteration is specified
    bool setIter = (atoi(argv[4]) != 0);

    // Read the period or the number of iterations to do in this simulation session
    int m_steps = 0;
    double m_time = 0;
    if (setIter)
    {
        m_steps = atoi(argv[3]);
    }
    else
    {
        m_time = atof(argv[3]);
    }
    
    // Read image drive parameters
    bool imageMode = false;
    bool recallMode = false;
    if (argc == 6)
    {
        imageMode = true;
        recallMode = (atoi(argv[5]) != 0);
    }
    
    pMat::pInMatFile = nullptr;
    pMat::pImageMatFile = nullptr;
    pMat::pIntermMatFile = nullptr;
    pMat::pOutMatFile = nullptr;

    if (myRank == MASTER_RANK)
    {
        // Open MAT-file "input.mat" for reading
        pMat::pInMatFile = matOpen(pMat::inMatFile, "r");
        if (pMat::pInMatFile == nullptr)
        {
            printf("\n>>>>> Failed to open input MAT-file for reading: %s\n", pMat::inMatFile);
            cout << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // If necessary, open MAT-file "image.mat" for reading
        if (imageMode)
        {
            pMat::pImageMatFile = matOpen(pMat::imageMatFile, "r");
            if (pMat::pImageMatFile == nullptr)
            {
                printf("\n>>>>> Failed to open image MAT-file for reading: %s\n", pMat::imageMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }

        // If necessary, open MAT-files "intermediate.mat" and "output.mat" for reading
        if (continuationMode)
        {
            pMat::pIntermMatFile = matOpen(pMat::intermMatFile, "r");
            if (pMat::pIntermMatFile == nullptr)
            {
                printf("\n>>>>> Failed to open intermediate MAT-file for reading: %s\n", pMat::intermMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }

            pMat::pOutMatFile = matOpen(pMat::outMatFile, "r");
            if (pMat::pOutMatFile == nullptr)
            {
                printf("\n>>>>> Failed to open output MAT-file for reading: %s\n", pMat::outMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }
    }

    // Create custom MPI types necessary to send/receive object of types std::mt19937 and std::mt19937_64 in graceful way
    MPI_Type_contiguous(sizeof(mt19937), MPI_BYTE, &MPI_MT19937);
    MPI_Type_commit(&MPI_MT19937);

    MPI_Type_contiguous(sizeof(mt19937_64), MPI_BYTE, &MPI_MT19937_64);
    MPI_Type_commit(&MPI_MT19937_64);

    // Is Single-Precision Arithmetics used?
    bool useSPA = ReadCheckBroadcastScalar<bool>("useSPA");

    // Should we use coarse-grained 32-bit Random Number Generator or fine-grained 64-bit one?
    // (There is the choice only if useSPA == false.)
    bool use32BitRng = ReadCheckBroadcastScalar<bool>("use32BitRng");
    
    if (useSPA)
    {
        main_templated<float, mt19937>(continuationMode, setIter, m_steps, (float)m_time, imageMode, recallMode);
    }
    else
    {
        if (use32BitRng)
        {
            main_templated<double, mt19937>(continuationMode, setIter, m_steps, m_time, imageMode, recallMode);
        }
        else
        {
            main_templated<double, mt19937_64>(continuationMode, setIter, m_steps, m_time, imageMode, recallMode);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    
    return 0;
}

template <typename T, typename RNGT>
void main_templated
(
    bool continuationMode,
    bool setIter,
    int m_steps,
    T m_time,
    bool imageMode,
    bool recallMode
)
{
    using namespace DistEnv;

    // Create instance of gamma simulator templated class
    GammaSimulator<T, RNGT> simulator;

    // Save parameters obtained via command line
    simulator.continuationMode = continuationMode;
    simulator.setIter = setIter;
    simulator.m_steps = m_steps;
    simulator.m_time = m_time;
    simulator.imageMode = imageMode;
    simulator.recallMode = recallMode;

    // Load data from input MAT-file,
    // if necessary, load data from image MAT-file,
    // allocate temporary arrays
    
    simulator.ReadInputDataAllocateTemporaryArrays();
    
    if (myRank == MASTER_RANK)
    {
        // Close MAT-file "input.mat"
        int status = matClose(pMat::pInMatFile);
        if (status != 0)
        {
            printf("\n>>>>> Failed to close input MAT-file: %s\n", pMat::inMatFile);
            cout << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // If necessary, close MAT-file "image.mat" after reading
        if (imageMode)
        {
            status = matClose(pMat::pImageMatFile);
            if (status != 0)
            {
                printf("\n>>>>> Failed to close image MAT-file after reading: %s\n", pMat::imageMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }
    
        // If necessary, close MAT-files "intermediate.mat" and "output.mat" after reading
        if (continuationMode)
        {
            status = matClose(pMat::pIntermMatFile);
            if (status != 0)
            {
                printf("\n>>>>> Failed to close intermediate MAT-file after reading: %s\n", pMat::intermMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }

            status = matClose(pMat::pOutMatFile);
            if (status != 0)
            {
                printf("\n>>>>> Failed to close output MAT-file after reading: %s\n", pMat::outMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }
    }

    // Run time evolution cycle
    simulator.DoSimulation();
    
    // Save files "output.mat" (always) and "intermediate.mat" (if saveIntermMat == true)
    simulator.SaveEndResults();
}