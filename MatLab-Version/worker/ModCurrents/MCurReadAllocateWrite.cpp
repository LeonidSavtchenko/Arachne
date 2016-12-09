#include "ModCurrent.h"
#include "MatFileIO/MatFileIOUtils.h"

#include <omp.h>

// Load data from input MAT-file
template <typename T, typename RNGT>
void ModCurrent<T, RNGT>::ReadInputDataAllocateTemporaryArrays(bool continuationMode, bool enable, int num, const DistVector<T> &v)
{
    // The current itself
    I = DistVector<T>(num);

    if (enable)
    {
        // Parameters of visualization
        watchedModCurrentNum = ReadCheckBroadcastScalar<int>("watchedModCurrentNum_" + suffix);

        if (watchedModCurrentNum != 0)
        {
            ReadTransitMxArrays();

            watchedModCurrentIdx = ReadCheckBroadcastVector<int>("watchedModCurrentIdx_" + suffix, watchedModCurrentNum);

            // Adjust 1-based Matlab indices to 0-based
            for (int i = 0; i < watchedModCurrentIdx.length; ++i)
            {
                --watchedModCurrentIdx[i];
            }

            watchedModCurrentI = LocalDenseMatrix<T, RNGT>(watchedModCurrentNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);

            if (!continuationMode)
            {
                I.CopySliceToMatrixColumn(watchedModCurrentI, 0, watchedModCurrentIdx);
                watchedModCurrentI.FillColFromVector(I, 0, true);
            }
            else
            {
                ReadCheckInMatrix<T, RNGT>(watchedModCurrentI, "watchedModCurrentI_" + suffix, watchedModCurrentNum, m_steps_prev + 1, MatFile::Output);
            }
        }

        // Initialize the current vector
        p_AllModCurrents->SetVoltage(v);
        I = p_AllModCurrents->GetSumCurrent(I.length);
    }
    else
    {
        #pragma omp parallel
        {
            I.AssignZeros();
        }
    }
}


// Read data from the input MAT-file to write in the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void ModCurrent<T, RNGT>::ReadTransitMxArrays()
{
    watchedModCurrentIdxOneBased = ReadMxArray("watchedModCurrentIdx_" + suffix);
}

// Write data to output MAT-file.
// The method is called just on master thread of master rank.
template <typename T, typename RNGT>
void ModCurrent<T, RNGT>::WriteOutputData(int num_steps)
{
    WriteScalar<int>(watchedModCurrentNum, "watchedModCurrentNum_" + suffix);

    if (watchedModCurrentNum != 0)
    {
        WriteTransitMxArrays();
        WriteCutMatrix<T, RNGT>(watchedModCurrentI, "watchedModCurrentI_" + suffix, num_steps + 1, MatFile::Output);
    }
}

// Transfer data from the input MAT-file to the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void ModCurrent<T, RNGT>::WriteTransitMxArrays()
{
    WriteMxArray(watchedModCurrentIdxOneBased, "watchedModCurrentIdx_" + suffix);
}


