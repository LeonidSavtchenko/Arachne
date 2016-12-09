#include "GammaSimulator.h"

#include "DistEnv.h"

#include <stdio.h>  // printf
#include <time.h>   // clock_t, CLOCKS_PER_SEC, clock()


#define SAFE_GATHER(xy)                                     \
{                                                           \
    auto ptr = dynamic_cast<SimpleSCM<T, RNGT>*>(g_##xy);   \
    if (ptr != nullptr)                                     \
    {                                                       \
        ptr->Gather(g_##xy##_local);                        \
    }                                                       \
}


// Print overall progress once per reportPeriodIter iterations.
// If we are running in background mode, refresh the file that indicates current status.
// The method is called on master thread of master process only.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ReportCurrentProgress(clock_t time)
{
    using namespace DistEnv;

    double iterDur;
    
    iterDur = double(clock() - time) / CLOCKS_PER_SEC;
#ifdef __linux__
    iterDur /= numThreads;
#endif
    if (!continuationMode)
    {
        // Number of this iteration
        // /
        // Total number of iterations to do
        // :
        // Duration of this iteration
        printf("iter %i / %i: %g sec\n", iter + 1, m_steps, iterDur);
    }
    else
    {
        // Global number of this iteration
        // (
        // Number of this iteration in this simulation session
        // /
        // Total number of iterations to do in this simulation session
        // ):
        // Duration of this iteration
        printf("iter %i (%i / %i): %g sec\n", iter + 1, iter + 1 - m_steps_prev, m_steps - m_steps_prev, iterDur);
    }
    cout << flush;

    if (backgroundMode)
    {
        ReportCurrentProgressBackground(iter, m_steps, iterDur, continuationMode, m_steps_prev);
    }
}

// Save backup files "output.mat" (always) and "intermediate.mat" (if saveIntermMat == true).
// The method is called on all threads of all processes.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::SaveCurrentProgress(bool snapshotRequested)
{
    using namespace DistEnv;

    const char *backupOutMatFile = nullptr;
    const char *backupIntermMatFile = nullptr;
    
    // If it is necessary, prepare matrices of synaptic conductance for output
    if ((enableSTDP && saveIntermMat) || gatherSCM)
    {
        SAFE_GATHER(ee)
        SAFE_GATHER(ei)
        SAFE_GATHER(ie)
        SAFE_GATHER(ii)
    }
    
    #pragma omp master
    {
        // Barrier before master's solo
        MPI_Barrier(MPI_COMM_WORLD);

        if (myRank == MASTER_RANK)
        {
            // Select backup files to save
            if (use1stBackupStorage)
            {
                backupOutMatFile = pMat::backupOutMatFile1;
                backupIntermMatFile = pMat::backupIntermMatFile1;
            }
            else
            {
                backupOutMatFile = pMat::backupOutMatFile2;
                backupIntermMatFile = pMat::backupIntermMatFile2;
            }

            if (snapshotRequested)
            {
                cout << "HPC kernel is dumping a snapshot per request of the host ...\n" << flush;
            }
            cout << "Saving backup file(s) ...\n" << flush;
            
            // Save results to backup output data MAT-file.
            // (The function is called just on master thread of master rank.)
            WriteOutputData(backupOutMatFile);
        }

        // Barrier after master's solo
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    // Barrier after master's solo
    #pragma omp barrier

    if (saveIntermMat)
    {
        // Save results to backup intermediate data MAT-file.
        // (The method is called on all ranks with any number of threads, but not inside "#pragma omp master" region.)
        GatherWriteIntermediateData(backupIntermMatFile);
    }
    
    // Barrier before master's solo
    #pragma omp barrier

    #pragma omp master
    {
        // Barrier before master's solo
        MPI_Barrier(MPI_COMM_WORLD);

        if (myRank == MASTER_RANK)
        {
            // Use other backup storage next time
            use1stBackupStorage = !use1stBackupStorage;
            
            // Update the pointer file whose name specifies the actual backup storage to recover from.
            // (The function is called on master thread of master process only.)
            SwitchTargetBackupStorage(use1stBackupStorage);
            
            char bsNumber = use1stBackupStorage ? '2' : '1';
            cout << "Backup file(s) saved to backup storage " << bsNumber << ".\n" << flush;
        }
        
        // Barrier after master's solo
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Barrier after master's solo
    #pragma omp barrier
}

// Save files "output.mat" (always) and "intermediate.mat" (if saveIntermMat == true) at the end of simulation.
// The method is called on master thread of all processes.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::SaveEndResults()
{
    using namespace DistEnv;

    // If it is necessary, prepare matrices of synaptic conductance for output
    if ((enableSTDP && saveIntermMat) || gatherSCM)
    {
        SAFE_GATHER(ee)
        SAFE_GATHER(ei)
        SAFE_GATHER(ie)
        SAFE_GATHER(ii)
    }
    
    // Barrier before master's solo
    MPI_Barrier(MPI_COMM_WORLD);

    if (myRank == MASTER_RANK)
    {
        WriteOutputData(pMat::outMatFile);
    }
        
    // Barrier after master's solo
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (saveIntermMat)
    {
        GatherWriteIntermediateData(pMat::intermMatFile);
    }
}
