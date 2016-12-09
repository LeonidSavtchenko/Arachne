#include "ExtraCurrent.h"
#include "MatFileIO/MatFileIOUtils.h"

#include <omp.h>

// Load data from input MAT-file
template <typename T, typename RNGT>
void ExtraCurrent<T, RNGT>::ReadInputDataAllocateTemporaryArrays(bool continuationMode, bool enable, int num, const DistVector<T> &v)
{
    // The current itself
    I = DistVector<T>(num);

    if (enable)
    {
        p = ReadCheckBroadcastScalar<int>("xcP_" + suffix);
        q = ReadCheckBroadcastScalar<int>("xcQ_" + suffix);

        g = ReadCheckBroadcastScalar<T>("xcG_" + suffix);
        v_r = ReadCheckBroadcastScalar<T>("xcVr_" + suffix);

        v_a = ReadCheckBroadcastScalar<T>("xcVa_" + suffix);
        s_a = ReadCheckBroadcastScalar<T>("xcSa_" + suffix);
        t_a = ReadCheckBroadcastScalar<T>("xcTa_" + suffix);

        v_b = ReadCheckBroadcastScalar<T>("xcVb_" + suffix);
        s_b = ReadCheckBroadcastScalar<T>("xcSb_" + suffix);
        t_b = ReadCheckBroadcastScalar<T>("xcTb_" + suffix);

        a_local = LocalVector<T>(num, AllocMode::onlyMasterMxCalloc);
        ReadCheckInVector(a_local, "xcA_" + suffix, num, MatFile::IntermInput);
        a = a_local.Scatter();

        b_local = LocalVector<T>(num, AllocMode::onlyMasterMxCalloc);
        ReadCheckInVector(b_local, "xcB_" + suffix, num, MatFile::IntermInput);
        b = b_local.Scatter();

        // Parameters of visualization
        watchedExtraCurrentNum = ReadCheckBroadcastScalar<int>("watchedExtraCurrentNum_" + suffix);

        if (watchedExtraCurrentNum != 0)
        {
            ReadTransitMxArrays();
            
            watchedExtraCurrentIdx = ReadCheckBroadcastVector<int>("watchedExtraCurrentIdx_" + suffix, watchedExtraCurrentNum);

            // Adjust 1-based Matlab indices to 0-based
            for (int i = 0; i < watchedExtraCurrentIdx.length; ++i)
            {
                --watchedExtraCurrentIdx[i];
            }

            watchedExtraCurrentI = LocalDenseMatrix<T, RNGT>(watchedExtraCurrentNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);
           
            if (!continuationMode)
            {
                I.CopySliceToMatrixColumn(watchedExtraCurrentI, 0, watchedExtraCurrentIdx);
                watchedExtraCurrentI.FillColFromVector(I, 0, true);
            }
            else
            {
                ReadCheckInMatrix<T, RNGT>(watchedExtraCurrentI, "watchedExtraCurrentI_" + suffix, watchedExtraCurrentNum, m_steps_prev + 1, MatFile::Output);
            }
        }

        // Initialize the current vector
        #pragma omp parallel
        {
            int localLength = I.localLength;

            int myThread = omp_get_thread_num();
            int startIdx = GetThreadChunkStartIdx(localLength, myThread);
            int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);
            for (int idx = startIdx; idx < endIdx; idx++)
            {
                I[idx] = ComputeExtraCurrent(a[idx], b[idx], v[idx]);
            }
        }

        a_tmp = DistVector<T>(num);
        b_tmp = DistVector<T>(num);
    }
    else
    {
        #pragma omp parallel
        {
            I.AssignZeros();
        }
    }
}

// Write data to intermediate MAT-file.
// The method is called on all ranks with any number of threads, but not inside "#pragma omp master" region.
template <typename T, typename RNGT>
void ExtraCurrent<T, RNGT>::GatherWriteIntermediateData()
{
    using namespace DistEnv;

    // Gather distributed vectors into local ones
    a.Gather(a_local, true);
    b.Gather(b_local, true);

    // Write vectors
    if (myRank == MASTER_RANK)
    {
        #pragma omp barrier
        #pragma omp master
        {
            WriteVector<T>(a_local, "xcA_" + suffix, MatFile::Intermediate);
            WriteVector<T>(b_local, "xcB_" + suffix, MatFile::Intermediate);
        }
        #pragma omp barrier
    }
}


// Read data from the input MAT-file to write in the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void ExtraCurrent<T, RNGT>::ReadTransitMxArrays()
{
    watchedExtraCurrentIdxOneBased = ReadMxArray("watchedExtraCurrentIdx_" + suffix);
}


// Write data to output MAT-file.
// The method is called just on master thread of master rank.
template <typename T, typename RNGT>
void ExtraCurrent<T, RNGT>::WriteOutputData(int num_steps)
{
    WriteScalar<int>(watchedExtraCurrentNum, "watchedExtraCurrentNum_" + suffix);
     
    if (watchedExtraCurrentNum != 0)
    {
        WriteTransitMxArrays();
        WriteCutMatrix<T, RNGT>(watchedExtraCurrentI, "watchedExtraCurrentI_" + suffix, num_steps + 1, MatFile::Output);
    }
}

// Transfer data from the input MAT-file to the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void ExtraCurrent<T, RNGT>::WriteTransitMxArrays()
{
    WriteMxArray(watchedExtraCurrentIdxOneBased, "watchedExtraCurrentIdx_" + suffix);
}


