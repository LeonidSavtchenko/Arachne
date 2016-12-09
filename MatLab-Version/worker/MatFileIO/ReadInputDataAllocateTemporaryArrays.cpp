#include "GammaSimulator.h"
#include "SCMs/SCMFactory.h"
#include "Containers/LocalVector.h"
#include "MatFileIOUtils.h"
#include "OtherFileIOUtils.h"
#include "DistEnv.h"
#include "SCMs/ScmType.h"
#include "SclModel.h"
#include "STDP/AnalyticStdpMechanism.h"
#include "STDP/TabulatedStdpMechanism.h"

#include <stdio.h>  // printf
#include <stdint.h> // uint8_t, uint64_t

#include <iostream> // cout, flush
#include <random>   // mt19937, mt19937_64

using namespace std;


// Load data from input MAT-file,
// if necessary, load data from image MAT-file,
// broadcast/scatter scalars, vectors, and matrices,
// allocate local and distributed vectors for temporary data.
// The method is called on all ranks with single thread.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ReadInputDataAllocateTemporaryArrays()
{
    using namespace DistEnv;

    if (myRank == MASTER_RANK)
    {
        // Read input data to transfer into output file
        ReadTransitMxArrays();
    }

    // Read scalars on master rank,
    // broadcast them to all ranks

    // Boolean scalars
    backgroundMode = ReadCheckBroadcastScalar<bool>("backgroundMode");
    stabAnalysis = ReadCheckBroadcastScalar<bool>("stabAnalysis");
    saveIntermMat = ReadCheckBroadcastScalar<bool>("saveIntermMat");
    saveBackupMats = ReadCheckBroadcastScalar<bool>("saveBackupMats");
    distMatPVH = ReadCheckBroadcastScalar<bool>("distMatPVH");
    enableSTDP = ReadCheckBroadcastScalar<bool>("enableSTDP");
    
    if (enableSTDP)
    {
        // Whether to import STDP models from text files
        bool importSTDP = ReadCheckBroadcastScalar<bool>("importSTDP");

        if (!importSTDP)
        {
            stdpMechanism_ee = new AnalyticStdpMechanism<T, RNGT>(dt, 0);
            stdpMechanism_ei = new AnalyticStdpMechanism<T, RNGT>(dt, 1);
            stdpMechanism_ie = new AnalyticStdpMechanism<T, RNGT>(dt, 2);
            stdpMechanism_ii = new AnalyticStdpMechanism<T, RNGT>(dt, 3);
        }
        else
        {
            stdpMechanism_ee = new TabulatedStdpMechanism<T, RNGT>(dt, 0);
            stdpMechanism_ei = new TabulatedStdpMechanism<T, RNGT>(dt, 1);
            stdpMechanism_ie = new TabulatedStdpMechanism<T, RNGT>(dt, 2);
            stdpMechanism_ii = new TabulatedStdpMechanism<T, RNGT>(dt, 3);
        }
    }

    if (myRank == MASTER_RANK && !enableSTDP && imageMode && !recallMode)
    {
        cout << "\n>>>>> The image cannot be memorized because STDP mechanism was not enabled before the first simulation session.\n" << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    gatherSCM = ReadCheckBroadcastScalar<bool>("gatherSCM");

    // Integer scalars
    ScmType scmType_ee = (ScmType)ReadCheckBroadcastScalar<int>("scmType_ee");
    ScmType scmType_ie = (ScmType)ReadCheckBroadcastScalar<int>("scmType_ie");
    ScmType scmType_ei = (ScmType)ReadCheckBroadcastScalar<int>("scmType_ei");
    ScmType scmType_ii = (ScmType)ReadCheckBroadcastScalar<int>("scmType_ii");

    reportPeriodIter = ReadCheckBroadcastScalar<int>("reportPeriodIter");
    c4cPeriodIter = ReadCheckBroadcastScalar<int>("c4cPeriodIter");
    if (saveBackupMats)
    {
        backupPeriodIter = ReadCheckBroadcastScalar<int>("backupPeriodIter");
    }
    num_e = ReadCheckBroadcastScalar<int>("num_e");
    num_i = ReadCheckBroadcastScalar<int>("num_i");
    sclModel = (SclModel)ReadCheckBroadcastScalar<int>("sclModel");
    
    delta_ee = ReadCheckBroadcastScalar<int>("delta_ee");
    delta_ei = ReadCheckBroadcastScalar<int>("delta_ei");
    delta_ie = ReadCheckBroadcastScalar<int>("delta_ie");
    delta_ii = ReadCheckBroadcastScalar<int>("delta_ii");
    max_delay_e = ReadCheckBroadcastScalar<int>("max_delay_e");
    max_delay_i = ReadCheckBroadcastScalar<int>("max_delay_i");
    
    double max_num_spikes_e_factor = ReadCheckScalar<double>("max_num_spikes_e_factor");    // No need to broadcast
    double max_num_spikes_i_factor = ReadCheckScalar<double>("max_num_spikes_i_factor");    // to other ranks besides master one
    
    // Check that numbers of columns in bit matrices are aligned properly.
    // (Matlab host does the alignment, so it's just a sanity check.)
    if (myRank == MASTER_RANK)
    {
        int blockSize_e = 1;
        int blockSize_i = 1;
        if (scmType_ee == ScmType::KrnOneBit || scmType_ei == ScmType::KrnOneBit)
        {
            // This mode requires num_e to be evenly divisible by 64
            blockSize_e = 64;
        }

        if (scmType_ie == ScmType::KrnOneBit || scmType_ii == ScmType::KrnOneBit)
        {
            // This mode requires num_i to be evenly divisible by 64
            blockSize_i = 64;
        }

        if (num_e % blockSize_e != 0 || num_i % blockSize_i != 0)
        {
            cout << "\n>>>>> Number of neurons (num_e or/and num_i) was not aligned in Matlab.\n" << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }
    
    // Floating-point scalars
    dt = ReadCheckBroadcastScalar<T>("dt");
    dt05 = dt / 2;
    v_rev_e = ReadCheckBroadcastScalar<T>("v_rev_e");
    v_rev_i = ReadCheckBroadcastScalar<T>("v_rev_i");
    g_stoch_e = ReadCheckBroadcastScalar<T>("g_stoch_e");
    g_stoch_i = ReadCheckBroadcastScalar<T>("g_stoch_i");
    T f_stoch_e = ReadCheckBroadcastScalar<T>("f_stoch_e");
    T f_stoch_i = ReadCheckBroadcastScalar<T>("f_stoch_i");
    stoch_e_upd_const = f_stoch_e * dt / 1000;
    stoch_i_upd_const = f_stoch_i * dt / 1000;
    r_e = ReadCheckBroadcastScalar<T>("r_e");
    r_i = ReadCheckBroadcastScalar<T>("r_i");
    g_e = ReadCheckBroadcastScalar<T>("g_e");
    StimInter = ReadCheckBroadcastScalar<T>("StimInter");
    frequencyParam = ReadCheckBroadcastScalar<T>("frequencyParam");
    syncoefParam = ReadCheckBroadcastScalar<T>("syncoefParam");
    
    // Kinetic parameters
    h_i_v_1 = ReadCheckBroadcastScalar<T>("h_i_v_1");
    h_i_v_2 = ReadCheckBroadcastScalar<T>("h_i_v_2");
    h_i_a_1 = ReadCheckBroadcastScalar<T>("h_i_a_1");
    h_i_a_2 = ReadCheckBroadcastScalar<T>("h_i_a_2");
    h_i_b_1 = ReadCheckBroadcastScalar<T>("h_i_b_1");
    h_i_b_2 = ReadCheckBroadcastScalar<T>("h_i_b_2");
    
    m_i_v_1 = ReadCheckBroadcastScalar<T>("m_i_v_1");
    m_i_v_2 = ReadCheckBroadcastScalar<T>("m_i_v_2");
    m_i_a_1 = ReadCheckBroadcastScalar<T>("m_i_a_1");
    m_i_a_2 = ReadCheckBroadcastScalar<T>("m_i_a_2");
    m_i_b_1 = ReadCheckBroadcastScalar<T>("m_i_b_1");
    m_i_b_2 = ReadCheckBroadcastScalar<T>("m_i_b_2");
    
    n_i_v_1 = ReadCheckBroadcastScalar<T>("n_i_v_1");
    n_i_v_2 = ReadCheckBroadcastScalar<T>("n_i_v_2");
    n_i_a_1 = ReadCheckBroadcastScalar<T>("n_i_a_1");
    n_i_a_2 = ReadCheckBroadcastScalar<T>("n_i_a_2");
    n_i_b_1 = ReadCheckBroadcastScalar<T>("n_i_b_1");
    n_i_b_2 = ReadCheckBroadcastScalar<T>("n_i_b_2");
    
    h_e_v_1 = ReadCheckBroadcastScalar<T>("h_e_v_1");
    h_e_v_2 = ReadCheckBroadcastScalar<T>("h_e_v_2");
    h_e_a_1 = ReadCheckBroadcastScalar<T>("h_e_a_1");
    h_e_a_2 = ReadCheckBroadcastScalar<T>("h_e_a_2");
    h_e_b_1 = ReadCheckBroadcastScalar<T>("h_e_b_1");
    h_e_b_2 = ReadCheckBroadcastScalar<T>("h_e_b_2");
    
    m_e_v_1 = ReadCheckBroadcastScalar<T>("m_e_v_1");
    m_e_v_2 = ReadCheckBroadcastScalar<T>("m_e_v_2");
    m_e_a_1 = ReadCheckBroadcastScalar<T>("m_e_a_1");
    m_e_a_2 = ReadCheckBroadcastScalar<T>("m_e_a_2");
    m_e_b_1 = ReadCheckBroadcastScalar<T>("m_e_b_1");
    m_e_b_2 = ReadCheckBroadcastScalar<T>("m_e_b_2");
    
    n_e_v_1 = ReadCheckBroadcastScalar<T>("n_e_v_1");
    n_e_v_2 = ReadCheckBroadcastScalar<T>("n_e_v_2");
    n_e_a_1 = ReadCheckBroadcastScalar<T>("n_e_a_1");
    n_e_a_2 = ReadCheckBroadcastScalar<T>("n_e_a_2");
    n_e_b_1 = ReadCheckBroadcastScalar<T>("n_e_b_1");
    n_e_b_2 = ReadCheckBroadcastScalar<T>("n_e_b_2");
    
    phi = ReadCheckBroadcastScalar<T>("phi");
    
 
    // Calculate the number of iterations
    if (!setIter)
    {
        m_steps = (int)floor(m_time / dt + (T)0.5);
    }

    // Read vectors on master rank,
    // scatter them to all ranks
    
    v_e = ReadCheckScatterVector<T>("v_e", num_e, MatFile::IntermInput);
    n_e = ReadCheckScatterVector<T>("n_e", num_e, MatFile::IntermInput);
    m_e = ReadCheckScatterVector<T>("m_e", num_e, MatFile::IntermInput);
    h_e = ReadCheckScatterVector<T>("h_e", num_e, MatFile::IntermInput);
    s_e = ReadCheckScatterVector<T>("s_e", num_e, MatFile::IntermInput);
    s_stoch_e = ReadCheckScatterVector<T>("s_stoch_e", num_e, MatFile::IntermInput);

    tau_r_e = ReadCheckScatterVector<T>("tau_r_e", num_e);
    tau_d_e = ReadCheckScatterVector<T>("tau_d_e", num_e);

    v_i = ReadCheckScatterVector<T>("v_i", num_i, MatFile::IntermInput);
    n_i = ReadCheckScatterVector<T>("n_i", num_i, MatFile::IntermInput);
    m_i = ReadCheckScatterVector<T>("m_i", num_i, MatFile::IntermInput);
    h_i = ReadCheckScatterVector<T>("h_i", num_i, MatFile::IntermInput);
    s_i = ReadCheckScatterVector<T>("s_i", num_i, MatFile::IntermInput);
    s_stoch_i = ReadCheckScatterVector<T>("s_stoch_i", num_i, MatFile::IntermInput);

    tau_r_i = ReadCheckScatterVector<T>("tau_r_i", num_i);
    tau_d_i = ReadCheckScatterVector<T>("tau_d_i", num_i);

    // Hebbian correction parameters
    if (enableSTDP)
    {
        stdpPeriodIter = ReadCheckBroadcastScalar<int>("stdpPeriodIter");
        copy_col_e = LocalDenseMatrix<T, RNGT>(num_e, numThreads);
        copy_col_i = LocalDenseMatrix<T, RNGT>(num_i, numThreads);
    
        // Allocate vectors for recent spikes
        t_e_spikes_recent = LocalVector<T>(num_e * stdpPeriodIter, AllocMode::allRanksNew);
        t_i_spikes_recent = LocalVector<T>(num_i * stdpPeriodIter, AllocMode::allRanksNew);
        idx_e_spikes_recent = LocalVector<int>(num_e * stdpPeriodIter, AllocMode::allRanksNew);
        idx_i_spikes_recent = LocalVector<int>(num_i * stdpPeriodIter, AllocMode::allRanksNew);

        spikes_single_e = DistVector<bool>(num_e);
        spikes_single_i = DistVector<bool>(num_i);

        if (!continuationMode)
        {
            // Allocate and initialize vectors for number of spikes on single neurons
            num_spikes_single_e = DistVector<int>(num_e);
            num_spikes_single_i = DistVector<int>(num_i);

            #pragma omp parallel
            {
                num_spikes_single_e.AssignZeros();
                num_spikes_single_i.AssignZeros();
            }
        }
        else
        {
            num_spikes_single_e = ReadCheckScatterVector<int>("num_spikes_single_e", num_e, MatFile::Intermediate);
            num_spikes_single_i = ReadCheckScatterVector<int>("num_spikes_single_i", num_i, MatFile::Intermediate);
        }

        if (saveIntermMat)
        {
            num_spikes_single_e_local = LocalVector<int>(num_e, AllocMode::onlyMasterMxCalloc);
            num_spikes_single_i_local = LocalVector<int>(num_i, AllocMode::onlyMasterMxCalloc);
        }

        // Allocate vectors for frequency of single neurons
        freq_single_i = DistVector<T>(num_i);
        freq_single_e = DistVector<T>(num_e);
    }
    
    // If necessary, read random number generator seeds
    DistVector<int> *pScmSeedsDist = nullptr;
    if (scmType_ee > ScmType::HstSparse || scmType_ie > ScmType::HstSparse ||
        scmType_ei > ScmType::HstSparse || scmType_ii > ScmType::HstSparse)
    {
        // Vector of random number generator seeds used to generate synaptic conductance matrices
        LocalVector<int> scmSeedsLocal = ReadCheckVector<int>("scmSeeds", numRanks * numThreads * 4);

        // Give numThreads * 4 seeds to each rank. The factor 4 stands for four matrices of synaptic conductance.
        static DistVector<int> scmSeedsDist = scmSeedsLocal.Scatter();

        pScmSeedsDist = &scmSeedsDist;

        // Remark:
        // Random number generator std::mt19937 accepts 32-bit seed, while std::mt19937_64 accepts 64-bit one.
        // Despite this, we initialize both generators with 32-bit seeds for simplicity.
    }

    // Allocate local matrices for gathering of synaptic conductance matrices
    if ((enableSTDP && saveIntermMat) || gatherSCM)
    {
        g_ee_local = LocalDenseMatrix<T, RNGT>(num_e, num_e, AllocModeMatrix::MasterMxCalloc);
        g_ei_local = LocalDenseMatrix<T, RNGT>(num_e, num_i, AllocModeMatrix::MasterMxCalloc);
        g_ie_local = LocalDenseMatrix<T, RNGT>(num_i, num_e, AllocModeMatrix::MasterMxCalloc);
        g_ii_local = LocalDenseMatrix<T, RNGT>(num_i, num_i, AllocModeMatrix::MasterMxCalloc);
    }
    
    // Create or read synaptic conductance matrices based on input or output MAT-file.
    // (Each random matrix created in HPC kernel is initialized with other set of seeds.)
    if (enableSTDP && continuationMode)
    {
        g_ee = ReadSimpleSCM<T, RNGT>("ee", num_e, num_e, sclModel, g_ee_local, gatherSCM);
        g_ei = ReadSimpleSCM<T, RNGT>("ei", num_e, num_i, sclModel, g_ei_local, gatherSCM);
        g_ie = ReadSimpleSCM<T, RNGT>("ie", num_i, num_e, sclModel, g_ie_local, gatherSCM);
        g_ii = ReadSimpleSCM<T, RNGT>("ii", num_i, num_i, sclModel, g_ii_local, gatherSCM);
    }
    else
    {
        bool useLocal = (enableSTDP && saveIntermMat) || gatherSCM;
        g_ee = CreateSCM<T, RNGT>("ee", num_e, num_e, sclModel, scmType_ee, pScmSeedsDist, g_ee_local, useLocal);
        g_ie = CreateSCM<T, RNGT>("ie", num_i, num_e, sclModel, scmType_ie, pScmSeedsDist, g_ie_local, useLocal);
        g_ei = CreateSCM<T, RNGT>("ei", num_e, num_i, sclModel, scmType_ei, pScmSeedsDist, g_ei_local, useLocal);
        g_ii = CreateSCM<T, RNGT>("ii", num_i, num_i, sclModel, scmType_ii, pScmSeedsDist, g_ii_local, useLocal);
    }
    
    // Prepare random number generators used to populate vectors u_e and u_i on each iteration
    if (!continuationMode)
    {
        // Create set of random number generators (numRanks ranks have numThreads generators each),
        // initialize them with the seeds.
        uGenStates = DistVector<RNGT>(numRanks * numThreads);
        releaseGenStates = DistVector<RNGT>(numRanks * numThreads);

        // Read random number generator SEEDS on master rank
        LocalVector<int> uSeedsLocal = ReadCheckVector<int>("uSeeds", numRanks * numThreads);
        LocalVector<int> releaseSeedsLocal = ReadCheckVector<int>("releaseSeeds", numRanks * numThreads);

        // Give numThreads seeds to each rank
        DistVector<int> uSeedsDist = uSeedsLocal.Scatter();
        DistVector<int> releaseSeedsDist = releaseSeedsLocal.Scatter();
        for (int i = 0; i < numThreads; i++)
        {
            uGenStates[i] = RNGT(uSeedsDist[i]);
            releaseGenStates[i] = RNGT(releaseSeedsDist[i]);
        }
    }
    else
    {
        // The check below is necessary because we'll deserialize and use the same set of random number generators of type std::mt19937 or std::mt19937_64
        // that was used in previous simulation session
        int totalNumThreads = ReadCheckScalar<int>("totalNumThreads", MatFile::Intermediate);
        if (myRank == MASTER_RANK && totalNumThreads != numRanks * numThreads)
        {
            printf("\n>>>>> Cannot continue previous simulation session because the product (np * nt) is not the same as before (i.e. not equal %i).\n", totalNumThreads);
            cout << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        // Read random number generator STATES on master rank
        LocalVector<uint8_t> uGenStatesLocal1 = ReadCheckVector<uint8_t>("uGenStates", numRanks * numThreads * sizeof(RNGT), MatFile::Intermediate);
        LocalVector<uint8_t> releaseGenStatesLocal1 = ReadCheckVector<uint8_t>("releaseGenStates", numRanks * numThreads * sizeof(RNGT), MatFile::Intermediate);

        // Little hack to make it possible to deserialize objects of type std::mt19937 or std::mt19937_64 from the MAT-file
        LocalVector<RNGT> uGenStatesLocal2;
        LocalVector<RNGT> releaseGenStatesLocal2;
        if (myRank == MASTER_RANK)
        {
            // !! TODO: Investigate why Linux compiler does not allow commented code while MSVS compiler allows
            RNGT *typeMarker = nullptr;
            uGenStatesLocal2 = uGenStatesLocal1.CastReshape(typeMarker);
            releaseGenStatesLocal2 = releaseGenStatesLocal1.CastReshape(typeMarker);
        }
        
        // Give numThreads states to each rank
        uGenStates = uGenStatesLocal2.Scatter();
        releaseGenStates = releaseGenStatesLocal2.Scatter();
    }

    // Create random number generator facade
    T lowerBound = (T)0;
    T upperBound = ReadCheckBroadcastScalar<T>("eUpperBound");
    zeroToMaxE = DistributionWrapper(lowerBound, upperBound);
    upperBound = ReadCheckBroadcastScalar<T>("iUpperBound");
    zeroToMaxI = DistributionWrapper(lowerBound, upperBound);

    if (continuationMode)
    {
        // The total number of iterations done on all previous simulation sessions
        m_steps_prev = ReadCheckBroadcastScalar<int>("m_steps", MatFile::Output);

        // The total number of iterations done on all previous simulation sessions plus this one
        // (if this simulation session completes without termination)
        m_steps += m_steps_prev;
    }
    else
    {
        // This is the first simulation session
        m_steps_prev = 0;
    }

    // Parameters of presynaptic voltage dynamics visualization:
    // watchedCellNum_*, watchedCellIdx_*, watchedCellIdxOneBased_*, watched_v_*
    ReadInputDataAllocateTemporaryArraysHelper1('e', v_e, watchedCellNum_e, watchedCellIdx_e, watchedCellIdxOneBased_e, watched_v_e);
    ReadInputDataAllocateTemporaryArraysHelper1('i', v_i, watchedCellNum_i, watchedCellIdx_i, watchedCellIdxOneBased_i, watched_v_i);

    // Parameters of synaptic conductance dynamics visualization:
    // watchedSynNum_**, watchedSynIdx_**, watchedSynIdxOneBased_**, watched_sc_**
    ReadInputDataAllocateTemporaryArraysHelper2("ee", g_ee, watchedSynNum_ee, watchedSynIdx_ee, watchedSynIdxOneBased_ee, watched_sc_ee);
    ReadInputDataAllocateTemporaryArraysHelper2("ei", g_ei, watchedSynNum_ei, watchedSynIdx_ei, watchedSynIdxOneBased_ei, watched_sc_ei);
    ReadInputDataAllocateTemporaryArraysHelper2("ie", g_ie, watchedSynNum_ie, watchedSynIdx_ie, watchedSynIdxOneBased_ie, watched_sc_ie);
    ReadInputDataAllocateTemporaryArraysHelper2("ii", g_ii, watchedSynNum_ii, watchedSynIdx_ii, watchedSynIdxOneBased_ii, watched_sc_ii);

    // Parameters of electroencephalography spectrum measurements
    opNum = ReadCheckBroadcastScalar<int>("opNum");
    if (opNum != 0)
    {
        radius_e = ReadCheckBroadcastScalar<T>("radius_e");
        radius_i = ReadCheckBroadcastScalar<T>("radius_i");
        electrolCond = ReadCheckBroadcastScalar<T>("electrolCond");
        opRadii = ReadCheckBroadcastVector<T>("opRadii", opNum);
        opAngles = ReadCheckBroadcastVector<T>("opAngles", opNum);

        // Initialize phi_e and phi_i
        phi_e = LocalDenseMatrix<T, RNGT>(opNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);
        phi_i = LocalDenseMatrix<T, RNGT>(opNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);
        if (continuationMode)
        {
            // Read data from the previous simulation session
            ReadCheckInMatrix(phi_e, "phi_e", opNum, m_steps_prev + 1, MatFile::Output);
            ReadCheckInMatrix(phi_i, "phi_i", opNum, m_steps_prev + 1, MatFile::Output);
        }
        else if (myRank == MASTER_RANK)
        {
            // The potentials are assumed equal zero on iteration 0
            memset(phi_e.data, 0, opNum * sizeof(T));
            memset(phi_i.data, 0, opNum * sizeof(T));
        }
    }

    freqWinWidth = ReadCheckBroadcastScalar<T>("freqWinWidth");
    freqWinWidth_iter = (int)floor(freqWinWidth / dt + T(0.5));

    // Gamma-Aminobutyric Acid mechanisms (DeltaVGABA, GTonicGABA, VTonicGABA)
    gaba = GABA<T>(num_i, m_steps_prev, m_steps, dt);
    gaba.ReadInputDataAllocateTemporaryArrays(continuationMode);

    if (enableSTDP)
    {
        if (saveIntermMat)
        {
            local_spikes_single_e_array = LocalDenseMatrix<bool, RNGT>(freqWinWidth_iter, num_e, AllocModeMatrix::MasterMxCalloc);
            local_spikes_single_i_array = LocalDenseMatrix<bool, RNGT>(freqWinWidth_iter, num_i, AllocModeMatrix::MasterMxCalloc);
        }

        if (!continuationMode)
        {
            // Allocate and initialize vectors for history of spikes on single neurons
            dist_spikes_single_e_array = DistDenseMatrix<bool, RNGT>(freqWinWidth_iter, num_e);
            dist_spikes_single_i_array = DistDenseMatrix<bool, RNGT>(freqWinWidth_iter, num_i);

            #pragma omp parallel
            {
                dist_spikes_single_e_array.FillZeros();
                dist_spikes_single_i_array.FillZeros();
            }            
        }
        else
        {
            // Read spikes of single neurons found on previous session
            dist_spikes_single_e_array = ReadCheckScatterMatrix<bool, RNGT>("spikes_single_e_array", freqWinWidth_iter, num_e, MatFile::Intermediate);
            dist_spikes_single_i_array = ReadCheckScatterMatrix<bool, RNGT>("spikes_single_i_array", freqWinWidth_iter, num_i, MatFile::Intermediate);
        }
    }

    // Allocate/read local vectors for gamma simulator output data.
    // (Only master rank will contain the vector to save in output MAT-file.)
    
    if (stabAnalysis)
    {
        // Allocate Frequency_* and syncoef_*
        Frequency_e = LocalVector<T>(m_steps, AllocMode::onlyMasterMxCalloc);
        Frequency_i = LocalVector<T>(m_steps, AllocMode::onlyMasterMxCalloc);
        syncoef_e = LocalVector<T>(m_steps, AllocMode::onlyMasterMxCalloc);
        syncoef_i = LocalVector<T>(m_steps, AllocMode::onlyMasterMxCalloc);
    }
    else if (gaba.enableFreqDelay)
    {
        Frequency_i = LocalVector<T>(m_steps, AllocMode::onlyMasterMxCalloc);
    }

    int max_num_spikes_e = int(num_e * m_steps * max_num_spikes_e_factor);
    int max_num_spikes_i = int(num_i * m_steps * max_num_spikes_i_factor);

    // Allocate idx_*_spikes and t_*_spikes
    idx_e_spikes = LocalVector<int>(max_num_spikes_e, AllocMode::onlyMasterMxCalloc);
    idx_i_spikes = LocalVector<int>(max_num_spikes_i, AllocMode::onlyMasterMxCalloc);
    t_e_spikes = LocalVector<T>(max_num_spikes_e, AllocMode::onlyMasterMxCalloc);
    t_i_spikes = LocalVector<T>(max_num_spikes_i, AllocMode::onlyMasterMxCalloc);
    
    if (!continuationMode)
    {
        // Initialize output scalars
        num_spikes_e = 0;
        num_spikes_i = 0;
    }
    else
    {
        // Read numbers of spikes found on previous simulation sessions
        num_spikes_e = ReadCheckBroadcastScalar<int>("num_spikes_e", MatFile::Output);
        num_spikes_i = ReadCheckBroadcastScalar<int>("num_spikes_i", MatFile::Output);

        // Read the vectors that were produced at time of previous simulation sessions,
        // copy their data to start of the new bigger arrays allocated above
        // !! TODO: Avoid this copying for Frequency_*, syncoef_*, idx_*_spikes, t_*_spikes
        
        // Initialize Frequency_* and syncoef_*
        if (stabAnalysis)
        {
            // Read the arrays with the data collected on previous simulation sessions
            LocalVector<T> Frequency_e_prev = ReadCheckVector<T>("Frequency_e", m_steps_prev, MatFile::Output);
            LocalVector<T> Frequency_i_prev = ReadCheckVector<T>("Frequency_i", m_steps_prev, MatFile::Output);
            LocalVector<T> syncoef_e_prev = ReadCheckVector<T>("syncoef_e", m_steps_prev, MatFile::Output);
            LocalVector<T> syncoef_i_prev = ReadCheckVector<T>("syncoef_i", m_steps_prev, MatFile::Output);

            if (myRank == MASTER_RANK)
            {
                // Copy the data to start of the new bigger arrays allocated above
                memcpy(Frequency_e.data, Frequency_e_prev.data, m_steps_prev * sizeof(T));
                memcpy(Frequency_i.data, Frequency_i_prev.data, m_steps_prev * sizeof(T));
                memcpy(syncoef_e.data, syncoef_e_prev.data, m_steps_prev * sizeof(T));
                memcpy(syncoef_i.data, syncoef_i_prev.data, m_steps_prev * sizeof(T));
            }
        }
        else if (gaba.enableFreqDelay)
        {
            // Read the array with the data collected on previous simulation sessions
            LocalVector<T> Frequency_i_prev = ReadCheckVector<T>("Frequency_i", m_steps_prev, MatFile::Output);

            if (myRank == MASTER_RANK)
            {
                // Copy the data to start of the new bigger array allocated above
                memcpy(Frequency_i.data, Frequency_i_prev.data, m_steps_prev * sizeof(T));
            }
        }

        // Initialize idx_*_spikes and t_*_spikes

        // Read arrays idx_*_spikes and t_*_spikes with the data collected on previous simulation sessions
        LocalVector<int> idx_e_spikes_prev = ReadCheckVector<int>("idx_e_spikes", num_spikes_e, MatFile::Output);
        LocalVector<int> idx_i_spikes_prev = ReadCheckVector<int>("idx_i_spikes", num_spikes_i, MatFile::Output);
        LocalVector<T> t_e_spikes_prev = ReadCheckVector<T>("t_e_spikes", num_spikes_e, MatFile::Output);
        LocalVector<T> t_i_spikes_prev = ReadCheckVector<T>("t_i_spikes", num_spikes_i, MatFile::Output);

        // Adjust 1-based Matlab indices to 0-based
        for (int i = 0; i < idx_e_spikes_prev.length; ++i)
        {
            --idx_e_spikes_prev[i];
        }
        for (int i = 0; i < idx_i_spikes_prev.length; ++i)
        {
            --idx_i_spikes_prev[i];
        }
        
        if (myRank == MASTER_RANK)
        {
            // Copy the data to start of the new bigger arrays for idx_*_spikes and t_*_spikes allocated above
            memcpy(idx_e_spikes.data, idx_e_spikes_prev.data, num_spikes_e * sizeof(int));
            memcpy(idx_i_spikes.data, idx_i_spikes_prev.data, num_spikes_i * sizeof(int));
            memcpy(t_e_spikes.data, t_e_spikes_prev.data, num_spikes_e * sizeof(T));
            memcpy(t_i_spikes.data, t_i_spikes_prev.data, num_spikes_i * sizeof(T));
        }
    }

    // Initialize sum_num_spikes_i and spikes_i_counter
    if (gaba.dynamicGTonicGABA && myRank == MASTER_RANK)
    {
        int bufsize = freqWinWidth_iter;
        if (gaba.enableFreqDelay)
        {
            bufsize += gaba.freqDelay;
        }
        sum_num_spikes_i = 0;
        if (!continuationMode)
        {
            // Allocate circular buffer for number of spikes on each iteration of freqWinWidth time interval
            spikes_i_counter = LocalVector<int>(bufsize, AllocMode::onlyMasterMxCalloc);
            spikes_i_counter.AssignZeros();
        }
        else
        {
            // Read the count of spikes collected on previous simulation sessions
            spikes_i_counter = ReadCheckVector<int>("spikes_i_counter", bufsize, MatFile::Intermediate);
            sum_num_spikes_i = ReadCheckScalar<int>("sum_num_spikes_i", MatFile::Intermediate);
        }
    }
    
    // Parameters of image memorization
    imageDrive = DistVector<T>(num_e);
    if (imageMode)
    {
        T blackValue = ReadCheckBroadcastScalar<T>("blackValue");
        T whiteValue = ReadCheckBroadcastScalar<T>("whiteValue");
        
        if (continuationMode)
        {
            numSession = ReadCheckBroadcastScalar<int>("numSession", MatFile::Output);
            numSession++;
            firstStepSession = LocalVector<int>(numSession, AllocMode::onlyMasterMxCalloc);
            ReadCheckInVector(firstStepSession, "firstStepSession", numSession - 1, MatFile::Output);
        }
        else
        {
            numSession = 1;
            firstStepSession = LocalVector<int>(numSession, AllocMode::onlyMasterMxCalloc);
        }
        
        modeSession = LocalVector<int>(numSession, AllocMode::onlyMasterMxCalloc);
        imageSession = LocalDenseMatrix<bool, RNGT>(num_e, numSession, AllocModeMatrix::MasterMxCalloc);
        if (continuationMode)
        {
            ReadCheckInVector(modeSession, "modeSession", numSession - 1, MatFile::Output);
            ReadCheckInMatrix(imageSession, "imageSession", num_e, numSession - 1, MatFile::Output);
        }
        
        LocalVector<bool> localImage = ReadCheckFixedSizeVector<bool>("image", num_e, MatFile::Image);

        if (myRank == MASTER_RANK)
        {
            // Save this image into the matrix containing all images
            memcpy(imageSession.data + num_e * (numSession - 1), localImage.data, num_e * sizeof(bool));

            firstStepSession[numSession - 1] = m_steps_prev + 1;
            modeSession[numSession - 1] = (recallMode) ? 1 : 0;
        }

        DistVector<bool> distImage = localImage.Scatter();
        for (int i = 0; i < distImage.localLength; i++)
        {
            imageDrive[i] = distImage[i] ? blackValue : whiteValue;
        }

        if (recallMode)
        {
            recall_frequency_e = LocalVector<T>(m_steps - m_steps_prev, AllocMode::onlyMasterMxCalloc);
            first_spike_recall = num_spikes_e + 1;
        }
    }
    else
    {
        #pragma omp parallel
        {
            imageDrive.AssignZeros();
        }
    }

    // Parameters of astrocyte
    enableAstro = ReadCheckBroadcastScalar<bool>("enableAstro");
    if (enableAstro)
    {
        T astroTimeScale = ReadCheckBroadcastScalar<T>("astroTimeScale");
        astro = AstroNet<T, RNGT>(num_e, m_steps_prev, m_steps, dt, astroTimeScale);
        astro.ReadInputDataAllocateTemporaryArrays(continuationMode, g_ei->rvg);
        
        lastSpikes_local = LocalVector<int>(num_e, AllocMode::onlyMasterMxCalloc);
        if (!continuationMode)
        {
            lastNumSpikes_e = 0;
            lastSpikes_e = DistVector<int>(num_e);
            #pragma omp parallel
            {
                lastSpikes_e.AssignZeros();
            }
        }
        else
        {
            lastNumSpikes_e = ReadCheckBroadcastScalar<int>("lastNumSpikes_e", MatFile::Intermediate);
            ReadCheckInVector(lastSpikes_local, "lastSpikes", num_e, MatFile::Intermediate);
            lastSpikes_e = lastSpikes_local.Scatter();
        }
    }

    // Allocate local vectors for temporary data.
    // (Only master rank allocates memory.)

    int size_e = int(num_e * ceil(4 * max_num_spikes_e / (T)num_e));
    int size_i = int(num_i * ceil(4 * max_num_spikes_i / (T)num_i));
    int size = max(size_e, size_i);
    results = LocalVector<T>(size, AllocMode::onlyMasterNew);

    local_freq_single_e = LocalVector<T>(num_e, AllocMode::allRanksNew);
    local_freq_single_i = LocalVector<T>(num_i, AllocMode::allRanksNew);

    // Local vectors for gathered data saved to "intermediate.mat"
    // !! TODO: Use these vectors when "intermediate.mat" or "input.mat" is read
    //          (no need to allocate memory twice)
    if (saveIntermMat)
    {
        v_e_local = LocalVector<T>(num_e, AllocMode::onlyMasterMxCalloc);
        n_e_local = LocalVector<T>(num_e, AllocMode::onlyMasterMxCalloc);
        m_e_local = LocalVector<T>(num_e, AllocMode::onlyMasterMxCalloc);
        h_e_local = LocalVector<T>(num_e, AllocMode::onlyMasterMxCalloc);
        s_e_local = LocalVector<T>(num_e, AllocMode::onlyMasterMxCalloc);
        s_stoch_e_local = LocalVector<T>(num_e, AllocMode::onlyMasterMxCalloc);

        v_i_local = LocalVector<T>(num_i, AllocMode::onlyMasterMxCalloc);
        n_i_local = LocalVector<T>(num_i, AllocMode::onlyMasterMxCalloc);
        m_i_local = LocalVector<T>(num_i, AllocMode::onlyMasterMxCalloc);
        h_i_local = LocalVector<T>(num_i, AllocMode::onlyMasterMxCalloc);
        s_i_local = LocalVector<T>(num_i, AllocMode::onlyMasterMxCalloc);
        s_stoch_i_local = LocalVector<T>(num_i, AllocMode::onlyMasterMxCalloc);

        uGenStates_local = LocalVector<RNGT>(numRanks * numThreads, AllocMode::onlyMasterMxCalloc);
        releaseGenStates_local = LocalVector<RNGT>(numRanks * numThreads, AllocMode::onlyMasterMxCalloc);
    }

    // Allocate distributed vectors for temporary data
    size = max(num_e, num_i);
    idx_spikes_iter = DistVector<int>(size);
    t_spikes_iter = DistVector<T>(size);
    I_e = DistVector<T>(num_e);
    I_i = DistVector<T>(num_i);
    gts1_e = DistVector<T>(num_e);
    gts2_e = DistVector<T>(num_e);
    gts1_i = DistVector<T>(num_i);
    gts2_i = DistVector<T>(num_i);
    s_e_tmp = DistVector<T>(num_e);
    s_i_tmp = DistVector<T>(num_i);
    u_e = DistVector<T>(num_e);
    u_i = DistVector<T>(num_i);
    v_e_old = DistVector<T>(num_e);
    v_i_old = DistVector<T>(num_i);
    v_e_tmp = DistVector<T>(num_e);
    n_e_tmp = DistVector<T>(num_e);
    m_e_tmp = DistVector<T>(num_e);
    h_e_tmp = DistVector<T>(num_e);
    v_i_tmp = DistVector<T>(num_i);
    n_i_tmp = DistVector<T>(num_i);
    m_i_tmp = DistVector<T>(num_i);
    h_i_tmp = DistVector<T>(num_i);
    
   // Allocate matrices of presynaptic voltage history
   if (distMatPVH)
   {
        dist_s_e_array = DistDenseMatrix<T, RNGT>(num_e, max_delay_e);
        dist_s_i_array = DistDenseMatrix<T, RNGT>(num_i, max_delay_i);
        dist_s_e_tmp_array = DistDenseMatrix<T, RNGT>(num_e, max_delay_e);
        dist_s_i_tmp_array = DistDenseMatrix<T, RNGT>(num_i, max_delay_i);
        dist_s_e_array.FillZeros();
        dist_s_i_array.FillZeros();
        dist_s_e_tmp_array.FillZeros();
        dist_s_i_tmp_array.FillZeros();
        
        currentIdx_e = 0;
        currentIdx_i = 0;
    }
    else
    {
        if (saveIntermMat)
        {
            local_s_e_array = LocalDenseMatrix<T, RNGT>(num_e, max_delay_e, AllocModeMatrix::MasterMxCallocOtherNew);
            local_s_i_array = LocalDenseMatrix<T, RNGT>(num_i, max_delay_i, AllocModeMatrix::MasterMxCallocOtherNew);
            local_s_e_tmp_array = LocalDenseMatrix<T, RNGT>(num_e, max_delay_e, AllocModeMatrix::MasterMxCallocOtherNew);
            local_s_i_tmp_array = LocalDenseMatrix<T, RNGT>(num_i, max_delay_i, AllocModeMatrix::MasterMxCallocOtherNew);
        }
        else
        {
            local_s_e_array = LocalDenseMatrix<T, RNGT>(num_e, max_delay_e);
            local_s_i_array = LocalDenseMatrix<T, RNGT>(num_i, max_delay_i);
            local_s_e_tmp_array = LocalDenseMatrix<T, RNGT>(num_e, max_delay_e);
            local_s_i_tmp_array = LocalDenseMatrix<T, RNGT>(num_i, max_delay_i);
        }
    
        if (continuationMode)
        {
            // Read circular buffers
            string name = "s_e_array";
            ReadCheckBroadcastInMatrix<T, RNGT>(local_s_e_array, name.c_str(), num_e, max_delay_e, MatFile::Intermediate);
            name = "s_i_array";
            ReadCheckBroadcastInMatrix<T, RNGT>(local_s_i_array, name.c_str(), num_i, max_delay_i, MatFile::Intermediate);
            name = "s_e_tmp_array";
            ReadCheckBroadcastInMatrix<T, RNGT>(local_s_e_tmp_array, name.c_str(), num_e, max_delay_e, MatFile::Intermediate);
            name = "s_i_tmp_array";
            ReadCheckBroadcastInMatrix<T, RNGT>(local_s_i_tmp_array, name.c_str(), num_i, max_delay_i, MatFile::Intermediate);
        
            // Read current indexes of circular buffers
            currentIdx_e = ReadCheckBroadcastScalar<int>("currentIdx_e", MatFile::Intermediate);
            currentIdx_i = ReadCheckBroadcastScalar<int>("currentIdx_i", MatFile::Intermediate);
        }
        else
        {
            local_s_e_array.FillZeros();
            local_s_i_array.FillZeros();
            local_s_e_tmp_array.FillZeros();
            local_s_i_tmp_array.FillZeros();
        
            currentIdx_e = 0;
            currentIdx_i = 0;
        }
    }

    if (distMatPVH)
    {
        s_e_array = &dist_s_e_array;
        s_i_array = &dist_s_i_array;
        s_e_tmp_array = &dist_s_e_tmp_array;
        s_i_tmp_array = &dist_s_i_tmp_array;
    }
    else
    {
        s_e_array = &local_s_e_array;
        s_i_array = &local_s_i_array;
        s_e_tmp_array = &local_s_e_tmp_array;
        s_i_tmp_array = &local_s_i_tmp_array;
    }
    
    buffer = LocalVector<T>(numThreads);

    if (myRank == MASTER_RANK)
    {
        if (saveBackupMats)
        {
            // Determine whether 1st or 2nd backup storage should be used for the first backup saving
            // (it switches after each backup saving)
            use1stBackupStorage = GetTargetBackupStorage(continuationMode);
        }
        else if (backgroundMode)
        {
            // Use 1st backup storage for the first snapshot dumping
            use1stBackupStorage = true;
        }
    }

    // Extra currents
    enableExtraCurrent_e = ReadCheckBroadcastScalar<bool>("enableExtraCurrent_e");
    enableExtraCurrent_i = ReadCheckBroadcastScalar<bool>("enableExtraCurrent_i");
    extraCurrent_e = ExtraCurrent<T, RNGT>('e', dt, continuationMode, enableExtraCurrent_e, num_e, v_e, m_steps_prev, m_steps);
    extraCurrent_i = ExtraCurrent<T, RNGT>('i', dt, continuationMode, enableExtraCurrent_i, num_i, v_i, m_steps_prev, m_steps);

    // Mod currents
    importMod_e = ReadCheckBroadcastScalar<bool>("importMod_e");
    importMod_i = ReadCheckBroadcastScalar<bool>("importMod_i");
    modCurrent_e = ModCurrent<T, RNGT>('e', dt, continuationMode, importMod_e, num_e, v_e, m_steps_prev, m_steps);
    modCurrent_i = ModCurrent<T, RNGT>('i', dt, continuationMode, importMod_i, num_i, v_i, m_steps_prev, m_steps);
}

template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ReadInputDataAllocateTemporaryArraysHelper1(
    char suffix,
    DistVector<T> &v,
    /*out*/ int &watchedCellNum,
    /*out*/ LocalVector<int> &watchedCellIdx,
    /*out*/ mxArray *&watchedCellIdxOneBased,
    /*out*/ LocalDenseMatrix<T, RNGT> &watched_v)
{
    std::string suf(1, suffix);

    watchedCellNum = ReadCheckBroadcastScalar<int>("watchedCellNum_" + suf);

    if (watchedCellNum != 0)
    {
        auto watchedCellIdx_varName = "watchedCellIdx_" + suf;

        watchedCellIdxOneBased = ReadMxArray(watchedCellIdx_varName);
        
        watchedCellIdx = ReadCheckBroadcastVector<int>(watchedCellIdx_varName, watchedCellNum);

        // Adjust 1-based Matlab indices to 0-based
        for (int i = 0; i < watchedCellIdx.length; ++i)
        {
            --watchedCellIdx[i];
        }
        
        watched_v = LocalDenseMatrix<T, RNGT>(watchedCellNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);
        
        if (!continuationMode)
        {
            // Initialize 0th column of watched_v
            v.CopySliceToMatrixColumn(watched_v, 0, watchedCellIdx);
        }
        else
        {
            // Read data from the previous simulation session
            ReadCheckInMatrix(watched_v, "watched_v_" + suf, watchedCellNum, m_steps_prev + 1, MatFile::Output);
        }
    }
    else
    {
        watchedCellIdx.length = 0;
    }
}

template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ReadInputDataAllocateTemporaryArraysHelper2(
    const char suffix[3],
    SCMBase<T, RNGT> *g,
    /*out*/ int &watchedSynNum,
    /*out*/ LocalDenseMatrix<int, RNGT> &watchedSynIdx,
    /*out*/ mxArray *&watchedSynIdxOneBased,
    /*out*/ LocalDenseMatrix<T, RNGT> &watched_sc)
{
    std::string suf = suffix;

    auto gMatrix = dynamic_cast<const DistDenseMatrix<T, RNGT>*>(g);
    if (gMatrix == nullptr)
    {
        watchedSynNum = 0;
        watchedSynIdx.numRows = 0;
        return;
    }
    
    watchedSynNum = ReadCheckBroadcastScalar<int>("watchedSynNum_" + suf);

    if (watchedSynNum != 0)
    {
        std::string watchedSynIdx_varName = "watchedSynIdx_" + suf;

        watchedSynIdxOneBased = ReadMxArray(watchedSynIdx_varName);

        watchedSynIdx = ReadCheckBroadcastMatrix<int, RNGT>(watchedSynIdx_varName, watchedSynNum, 2);

        // Adjust 1-based Matlab indices to 0-based
        int len = watchedSynIdx.numRows * watchedSynIdx.numCols;
        for (int i = 0; i < len; ++i)
        {
            --watchedSynIdx.data[i];
        }

        watched_sc = LocalDenseMatrix<T, RNGT>(watchedSynNum, m_steps + 1, AllocModeMatrix::MasterMxCalloc);
        if (!continuationMode)
        {
            // Initialize 0th column of watched_sc
            gMatrix->CopySliceToMatrixColumn(watched_sc, 0, watchedSynIdx);
        }
        else
        {
            // Read data from the previous simulation session
            ReadCheckInMatrix(watched_sc, "watched_sc_" + suf, watchedSynNum, m_steps_prev + 1, MatFile::Output);
        }
    }
    else
    {
        watchedSynIdx.numRows = 0;
    }
}

// Read data from input MAT-file to write in the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ReadTransitMxArrays()
{
    outFileName = ReadMxArray("outFileName");
    saveInput2Output = ReadMxArray("saveInput2Output");

    // watchedCellIdxOneBased_* are read in ReadInputDataAllocateTemporaryArraysHelper1.
    // watchedSynIdxOneBased_** are read in ReadInputDataAllocateTemporaryArraysHelper2.
}