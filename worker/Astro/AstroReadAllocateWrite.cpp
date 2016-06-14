#include "AstroNet.h"
#include "MatFileIO/MatFileIOUtils.h"

// Load data from input MAT-file,
// broadcast/scatter scalars, vectors, and matrices,
// allocate local and distributed vectors for temporary data.
// The method is called on all ranks with single thread.
template <typename T, typename RNGT>
void AstroNet<T, RNGT>::ReadInputDataAllocateTemporaryArrays(bool continuationMode, std::vector<BoostedBernoulliDistribution<T, RNGT>> &rvg)
{
    dt05 = dt / 2;

    // Read float-point scalars
    v1 = ReadCheckBroadcastScalar<T>("v1");
    v2 = ReadCheckBroadcastScalar<T>("v2");
    v3 = ReadCheckBroadcastScalar<T>("v3");
    d1 = ReadCheckBroadcastScalar<T>("d1");
    d2 = ReadCheckBroadcastScalar<T>("d2");
    d3 = ReadCheckBroadcastScalar<T>("d3");
    d5 = ReadCheckBroadcastScalar<T>("d5");
    c1 = ReadCheckBroadcastScalar<T>("c1");
    a2 = ReadCheckBroadcastScalar<T>("a2");
    k3 = ReadCheckBroadcastScalar<T>("k3");
    ip3star = ReadCheckBroadcastScalar<T>("ip3star");
    tip3 = ReadCheckBroadcastScalar<T>("tip3");
    rip3 = ReadCheckBroadcastScalar<T>("rip3");
    gs = ReadCheckBroadcastScalar<T>("gs");
    tau_spike = ReadCheckBroadcastScalar<T>("tau_spike");
    t_in = ReadCheckBroadcastScalar<T>("t_in");
    p_basic = ReadCheckBroadcastScalar<T>("p_basic");
    CaBA = ReadCheckBroadcastScalar<T>("CaBA");
    Ca_ER = ReadCheckBroadcastScalar<T>("Ca_ER");

    // Read and scatter vectors
    Ca_local = LocalVector<T>(num_a, AllocMode::onlyMasterMxCalloc);
    ip3_local = LocalVector<T>(num_a, AllocMode::onlyMasterMxCalloc);
    q_local = LocalVector<T>(num_a, AllocMode::onlyMasterMxCalloc);
    y_local = LocalVector<T>(num_a, AllocMode::onlyMasterMxCalloc);
    ReadCheckInVector(Ca_local, "Ca", num_a, MatFile::IntermInput);
    ReadCheckInVector(ip3_local, "ip3", num_a, MatFile::IntermInput);
    ReadCheckInVector(q_local, "q", num_a, MatFile::IntermInput);
    ReadCheckInVector(y_local, "y", num_a, MatFile::IntermInput);
    Ca = Ca_local.Scatter();
    ip3 = ip3_local.Scatter();
    q = q_local.Scatter();
    y = y_local.Scatter();

    // Allocate distributed vectors for temporary data
    Ca_tmp = DistVector<T>(num_a);
    ip3_tmp = DistVector<T>(num_a);
    q_tmp = DistVector<T>(num_a);
    y_tmp = y.Clone();  // We need to initialize y_tmp because it is read

    Ca_buf = LocalVector<T>(num_a);

    // Parameters of visualization
    watchedAstroNum = ReadCheckBroadcastScalar<int>("watchedAstroNum");
    gatherCaColormap = ReadCheckBroadcastScalar<bool>("gatherCaColormap");
    
    // Curves of Ca, release probability "e->i" and activation parameter "y"
    if (watchedAstroNum != 0)
    {
        ReadTransitMxArrays();

        watchedAstroIdx = ReadCheckBroadcastVector<int>("watchedAstroIdx", watchedAstroNum);

        // Adjust 1-based Matlab indices to 0-based
        for (int i = 0; i < watchedAstroIdx.length; ++i)
        {
            --watchedAstroIdx[i];
        }

        watchedAstroCa = LocalDenseMatrix<T, RNGT>(watchedAstroNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);
        watchedAstroY = LocalDenseMatrix<T, RNGT>(watchedAstroNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);
        watchedProb = LocalDenseMatrix<T, RNGT>(watchedAstroNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);

        if (!continuationMode)
        {
            // Write the initial condition values into the first columns
            // (p is set later)
            Ca.CopySliceToMatrixColumn(watchedAstroCa, 0, watchedAstroIdx);
            y.CopySliceToMatrixColumn(watchedAstroY, 0, watchedAstroIdx);
        }
        else
        {
            ReadCheckInMatrix<T, RNGT>(watchedAstroCa, "watchedAstroCa", watchedAstroNum, m_steps_prev + 1, MatFile::Output);
            ReadCheckInMatrix<T, RNGT>(watchedAstroY, "watchedAstroY", watchedAstroNum, m_steps_prev + 1, MatFile::Output);
            ReadCheckInMatrix<T, RNGT>(watchedProb, "watchedProb", watchedAstroNum, m_steps_prev + 1, MatFile::Output);
        }
    }
    
    // Specify release probabilities
    #pragma omp parallel
    {
        CorrectReleaseProbability(rvg, m_steps_prev - 1);
    }
    
    // Ca colormap
    if (gatherCaColormap)
    {
        caColormapPeriodIter = ReadCheckBroadcastScalar<int>("caColormapPeriodIter");

        int numCols = 1 + m_steps / caColormapPeriodIter;
        CaColormap = LocalDenseMatrix<T, RNGT>(num_a, numCols, AllocModeMatrix::MasterMxCalloc);

        if (!continuationMode)
        {
            // Initial condition
            CaColormap.FillColFromVector(Ca, 0, true);
        }
        else
        {
            int numCols_prev = 1 + m_steps_prev / caColormapPeriodIter;
            ReadCheckInMatrix<T, RNGT>(CaColormap, "CaColormap", num_a, numCols_prev, MatFile::Output);
        }
    }
    
}

// Read data from the input MAT-file to write in the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void AstroNet<T, RNGT>::ReadTransitMxArrays()
{
    watchedAstroIdxOneBased = ReadMxArray("watchedAstroIdx");
}

// Write data to output MAT-file.
// The method is called just on master thread of master rank.
template <typename T, typename RNGT>
void AstroNet<T, RNGT>::WriteOutputData(int num_steps)
{
    WriteScalar<int>(watchedAstroNum, "watchedAstroNum");
    if (watchedAstroNum != 0)
    {
        WriteTransitMxArrays();
        WriteCutMatrix<T, RNGT>(watchedAstroCa, "watchedAstroCa", num_steps + 1, MatFile::Output);
        WriteCutMatrix<T, RNGT>(watchedAstroY, "watchedAstroY", num_steps + 1, MatFile::Output);
        WriteCutMatrix<T, RNGT>(watchedProb, "watchedProb", num_steps + 1, MatFile::Output);
    }
    
    WriteScalar<bool>(gatherCaColormap, "gatherCaColormap");
    if (gatherCaColormap)
    {
        int numCols = 1 + num_steps / caColormapPeriodIter;
        WriteCutMatrix<T, RNGT>(CaColormap, "CaColormap", numCols, MatFile::Output);
    }
}

// Transfer data from the input MAT-file to the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void AstroNet<T, RNGT>::WriteTransitMxArrays()
{
    WriteMxArray(watchedAstroIdxOneBased, "watchedAstroIdx");
}

// Write data to intermediate MAT-file.
// The method is called on all ranks with any number of threads, but not inside "#pragma omp master" region.
template <typename T, typename RNGT>
void AstroNet<T, RNGT>::GatherWriteIntermediateData()
{
    using namespace DistEnv;
    
    // Gather distributed vectors into local ones
    Ca.Gather(Ca_local, true);
    y.Gather(y_local, true);
    ip3.Gather(ip3_local, true);
    q.Gather(q_local, true);
    
    // Write vectors
    if (myRank == MASTER_RANK)
    {
        #pragma omp barrier
        
        #pragma omp master
        {
            WriteVector<T>(Ca_local, "Ca", MatFile::Intermediate);
            WriteVector<T>(y_local, "y", MatFile::Intermediate);
            WriteVector<T>(ip3_local, "ip3", MatFile::Intermediate);
            WriteVector<T>(q_local, "q", MatFile::Intermediate);
        }

        #pragma omp barrier
    }
}