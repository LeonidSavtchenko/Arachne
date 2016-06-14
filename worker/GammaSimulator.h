#pragma once

#include <mat.h>
#include <omp.h>
#include "Astro/AstroNet.h"
#include "ExtraCurrent/ExtraCurrent.h"
#include "GABA/GABA.h"
#include "Containers/LocalVector.h"
#include "Containers/DistVector.h"
#include "Containers/LocalDenseMatrix.h"
#include "Containers/DistDenseMatrix.h"
#include "SCMs/SCMBase.h"
#include "DistributionWrapper.h"
#include "SclModel.h"
#include "STDP/BaseStdpMechanism.h"

#include <random>   // mt19937, mt19937_64

#include <time.h>   // clock_t

template <typename T, typename RNGT>
class GammaSimulator
{
public:

    // Load data from input MAT-file,
    // if necessary, load data from image MAT-file,
    // broadcast/scatter scalars, vectors, and matrices,
    // allocate local and distributed vectors for temporary data.
    // The method is called on all ranks.
    void ReadInputDataAllocateTemporaryArrays();

    // The wrapper of the time evolution cycle.
    // The method is called on all ranks in single thread.
    void DoSimulation();

    // Write data to output MAT-file.
    // The method is called just on master rank.
    void WriteOutputData(const char *outMatFile);

    // Write data to intermediate MAT-file.
    // The method is called on all ranks with any number of threads, but not inside "#pragma omp master" region.
    void GatherWriteIntermediateData(const char *intermMatFile);
    
    // Write data to intermediate MAT-file
    void SaveEndResults();

public:

    // Parameters obtained via command line
    bool continuationMode;  // Whether previous simulation session should be continued
    bool setIter;           // Whether the number of steps or the period is specified
    int m_steps;            // The number of steps to do in this simulation session
    T m_time;               // The period of this simulation session

    // Other public parameters
    bool saveIntermMat;     // Do we need to save "intermediate.mat"?
    bool imageMode;         // Whether to apply image drive to e-cells
    bool recallMode;        // Whether to disable STDP for this simulation session (image recall mode)

private:
    
#pragma region Private data

    // Input scalars

    bool backgroundMode;
    bool stabAnalysis;
    bool saveBackupMats;        // Whether backup file(s) should be saved periodically
    bool use1stBackupStorage;   // Whether 1st or 2nd backup storage should be used for the next backup saving
    bool distMatPVH;
    bool enableSTDP;
    bool gatherSCM;
    bool enableAstro;
    bool enableExtraCurrent_e, enableExtraCurrent_i;
    
    BaseStdpMechanism<T, RNGT> *stdpMechanism_ee;
    BaseStdpMechanism<T, RNGT> *stdpMechanism_ii;
    BaseStdpMechanism<T, RNGT> *stdpMechanism_ei;
    BaseStdpMechanism<T, RNGT> *stdpMechanism_ie;

    int reportPeriodIter;   // Progress reporting period (in iterations)
    int c4cPeriodIter;      // Period of checking for a command from Matlab host in background mode (in iterations)
    int backupPeriodIter;   // Backup file(s) saving period (in iterations)
    int num_e;
    int num_i;
    int m_steps_prev;       // Total number of steps done on all previous simulation sessions before this one
    
    SclModel sclModel;

    int delta_ee;
    int delta_ei;
    int delta_ie;
    int delta_ii;
    int currentIdx_e;
    int currentIdx_i;
    int max_delay_e;
    int max_delay_i;
    
    T dt;
    T dt05;
    T v_rev_e;
    T v_rev_i;
    T g_stoch_e;
    T g_stoch_i;
    T stoch_e_upd_const;    // = f_stoch_e * dt / 1000
    T stoch_i_upd_const;    // = f_stoch_i * dt / 1000
    T r_e;
    T r_i;
    T g_e;
    T StimInter;
    T frequencyParam, syncoefParam;
    
    // Kinetic parameters
    T h_i_v_1, m_i_v_1, n_i_v_1, h_e_v_1, m_e_v_1, n_e_v_1;
    T h_i_v_2, m_i_v_2, n_i_v_2, h_e_v_2, m_e_v_2, n_e_v_2;
    T h_i_a_1, m_i_a_1, n_i_a_1, h_e_a_1, m_e_a_1, n_e_a_1;
    T h_i_a_2, m_i_a_2, n_i_a_2, h_e_a_2, m_e_a_2, n_e_a_2;
    T h_i_b_1, m_i_b_1, n_i_b_1, h_e_b_1, m_e_b_1, n_e_b_1;
    T h_i_b_2, m_i_b_2, n_i_b_2, h_e_b_2, m_e_b_2, n_e_b_2;
    T phi;

    // Other scalars
    int iter;   // Current iteration (sessions-through number)

    // Input vectors
    DistVector<T> v_e, n_e, m_e, h_e, s_e, tau_r_e, tau_d_e, s_stoch_e;
    DistVector<T> v_i, n_i, m_i, h_i, s_i, tau_r_i, tau_d_i, s_stoch_i;

    // Input matrices
    // (base distributed matrix class pointers)
    SCMBase<T, RNGT> *g_ee, *g_ie, *g_ei, *g_ii;

    // Output scalars
    int num_spikes_e;
    int num_spikes_i;
    T Frequency_e_scalar, Frequency_i_scalar, syncoef_e_scalar, syncoef_i_scalar;   // Saved if stabAnalysis == false

    // Width of time window used for calculation of spikes frequencies in the following two cases:
    // 1) calculation of overall i-network frequency if dynamicGTonicGABA == true;
    // 2) calculation of particular neurons frequencies if enableSTDP == true.
    int freqWinWidth_iter;
    T freqWinWidth;

    // Output vectors
    DistVector<bool> spikes_single_i, spikes_single_e;
    DistVector<int> num_spikes_single_i, num_spikes_single_e;                       // Saved if enableSTDP == true
    LocalVector<T> Frequency_e, Frequency_i, syncoef_e, syncoef_i;                  // Saved if stabAnalysis == true
    LocalVector<T> t_e_spikes, t_i_spikes;
    LocalVector<int> idx_e_spikes, idx_i_spikes;

    // Temporary data scalars
    T t_old;
    T t_new;
    int sum_num_spikes_i;

    // mxArrays to transfer from input to the output MAT-file
    mxArray *outFileName;
    mxArray *saveInput2Output;
    
    // Temporary data vectors
    LocalVector<T> results;
    LocalVector<int> num_spikes_single_e_local, num_spikes_single_i_local;
    LocalVector<T> local_freq_single_e, local_freq_single_i;
    LocalVector<int> spikes_i_counter;
    DistVector<int> idx_spikes_iter;
    DistVector<T> t_spikes_iter;
    DistVector<T> I_e, I_i;
    DistVector<T> gts1_e, gts2_e, gts1_i, gts2_i;
    DistVector<T> s_e_tmp, s_i_tmp;
    DistVector<T> u_e, u_i;
    DistVector<T> v_e_old, v_i_old;
    DistVector<T> v_e_tmp, n_e_tmp, m_e_tmp, h_e_tmp;
    DistVector<T> v_i_tmp, n_i_tmp, m_i_tmp, h_i_tmp;
    DistVector<T> freq_single_i, freq_single_e;

    // Local vectors for gathered data saved to "intermediate.mat"
    LocalVector<T> v_e_local, n_e_local, m_e_local, h_e_local, s_e_local, s_stoch_e_local;
    LocalVector<T> v_i_local, n_i_local, m_i_local, h_i_local, s_i_local, s_stoch_i_local;
    LocalVector<RNGT> uGenStates_local;
    LocalVector<RNGT> releaseGenStates_local;
        
    // Presynaptic voltage history matrices
    LocalDenseMatrix<T, RNGT> local_s_e_array;
    LocalDenseMatrix<T, RNGT> local_s_i_array;
    LocalDenseMatrix<T, RNGT> local_s_e_tmp_array;
    LocalDenseMatrix<T, RNGT> local_s_i_tmp_array;
    DistDenseMatrix<T, RNGT> dist_s_e_array;
    DistDenseMatrix<T, RNGT> dist_s_i_array;
    DistDenseMatrix<T, RNGT> dist_s_e_tmp_array;
    DistDenseMatrix<T, RNGT> dist_s_i_tmp_array;
    DenseMatrix<T> *s_e_array, *s_i_array, *s_e_tmp_array, *s_i_tmp_array;

    // Matrices for history of spikes on single neurons
    LocalDenseMatrix<bool, RNGT> local_spikes_single_e_array;
    LocalDenseMatrix<bool, RNGT> local_spikes_single_i_array;
    DistDenseMatrix<bool, RNGT> dist_spikes_single_e_array;
    DistDenseMatrix<bool, RNGT> dist_spikes_single_i_array;
    
    // Hebbian correction variables
    int num_e_old;
    int num_i_old;
    int length_e;
    int length_i;
    int stdpPeriodIter;
    LocalVector<T> t_e_spikes_recent;
    LocalVector<T> t_i_spikes_recent;
    LocalVector<int> idx_e_spikes_recent;
    LocalVector<int> idx_i_spikes_recent;
    LocalDenseMatrix<T, RNGT> copy_col_e;
    LocalDenseMatrix<T, RNGT> copy_col_i;
    
    // Matrices for gathering
    LocalDenseMatrix<T, RNGT> g_ee_local, g_ei_local, g_ie_local, g_ii_local;

    // Random number generator staff
    DistributionWrapper zeroToMaxE, zeroToMaxI;
    DistVector<RNGT> uGenStates;
    DistVector<RNGT> releaseGenStates;

    // Variables for visualization of presynaptic voltage dynamics
    int watchedCellNum_e, watchedCellNum_i;
    LocalVector<int> watchedCellIdx_e, watchedCellIdx_i;
    LocalDenseMatrix<T, RNGT> watched_v_e, watched_v_i;
    
    // Variables for visualization of synaptic conductance dynamics
    int watchedSynNum_ee, watchedSynNum_ei, watchedSynNum_ie, watchedSynNum_ii;
    LocalDenseMatrix<int, RNGT> watchedSynIdx_ee, watchedSynIdx_ei, watchedSynIdx_ie, watchedSynIdx_ii;
    LocalDenseMatrix<T, RNGT> watched_sc_ee, watched_sc_ei, watched_sc_ie, watched_sc_ii;

    // Input MAT file verbatims
    mxArray *watchedCellIdxOneBased_e, *watchedCellIdxOneBased_i;
    mxArray *watchedSynIdxOneBased_ee, *watchedSynIdxOneBased_ei, *watchedSynIdxOneBased_ie, *watchedSynIdxOneBased_ii;

    // Variables for electroencephalography spectrum measurements
    int opNum;
    T radius_e, radius_i, electrolCond;
    LocalVector<T> opRadii, opAngles;
    LocalDenseMatrix<T, RNGT> phi_e, phi_i;
    LocalVector<T> buffer;
    
    // Variables of image memorization
    int numSession, first_spike_recall;
    DistVector<T> imageDrive;
    LocalVector<T> recall_frequency_e;
    LocalVector<int> firstStepSession, modeSession;
    LocalDenseMatrix<bool, RNGT> imageSession;
    
    // Gamma-Aminobutyric Acid mechanisms (DeltaVGABA, GTonicGABA, VTonicGABA)
    GABA<T> gaba;
    
    // Astrocyte network
    AstroNet<T, RNGT> astro;

    // Extra currents
    ExtraCurrent<T, RNGT> extraCurrent_e;
    ExtraCurrent<T, RNGT> extraCurrent_i;

    LocalVector<int> lastSpikes_local;
    DistVector<int> lastSpikes_e;
    int lastNumSpikes_e;

#pragma endregion

#pragma region Private methods

    // The time evolution cycle.
    // The method is called on all ranks in multiple threads.
    void DoSimulationMultithreaded();

    // Read data from the input MAT-file to write in the output MAT-file.
    // The method is called just on master rank.
    void ReadTransitMxArrays();

    // Transfer data from the input MAT-file to the output MAT-file.
    // The method is called just on master rank.
    void WriteTransitMxArrays();

    void ReadInputDataAllocateTemporaryArraysHelper1(
        char suffix,
        DistVector<T> &v,
        /*out*/ int &watchedCellNum,
        /*out*/ LocalVector<int> &watchedCellIdx,
        /*out*/ mxArray *&watchedCellIdxOneBased,
        /*out*/ LocalDenseMatrix<T, RNGT> &watched_v);
    
    void ReadInputDataAllocateTemporaryArraysHelper2(
        const char suffix[3],
        SCMBase<T, RNGT> *g,
        /*out*/ int &watchedSynNum,
        /*out*/ LocalDenseMatrix<int, RNGT> &watchedSynIdx,
        /*out*/ mxArray *&watchedSynIdxOneBased,
        /*out*/ LocalDenseMatrix<T, RNGT> &watched_sc);

    void ComputeExternalDrive_e(T t, bool firstCall = false);
    void ComputeExternalDrive_i(T t, bool firstCall = false);

    void ElementwiseOperation1();
    void ElementwiseOperation2();
    void ElementwiseOperation3();
    void ElementwiseOperation4();

    // Determine which and how many cells of given type (E or I) spiked in the current time step.
    // Update { idx_e_spikes, t_e_spikes, num_spikes_e } or { idx_i_spikes, t_i_spikes, num_spikes_i } living on master rank.
    // Update spikes on single neurons
    void UpdateIdxTNumSpikes
    (
        const DistVector<T> &v,
        const DistVector<T> &v_old,
        LocalVector<int> &idx_spikes,
        LocalVector<T> &t_spikes,
        DistVector<bool> &spikes_single,
        int &num_spikes,
        char letter
    );

    // Given rank, analyse local portions of vectors v and v_old and determine if any spikes appear.
    // If so, populate local portions of vectors idx_spikes_iter and t_spikes_iter
    // (the positions with indices 0, 1, ..., num_spikes_local - 1 are populated).
    // Return number of spikes num_spikes_local found on this rank.
    int SelectSpikes
    (
        const DistVector<T> &v,
        const DistVector<T> &v_old,
        DistVector<int> &idx_spikes_iter,
        DistVector<T> &t_spikes_iter
    );

    // Compute syncoef_e or syncoef_i.
    // The method is called in master thread of master process only.
    T ComputeSynCoef
    (
        T Frequency,
        int num,
        int num_spikes,
        const LocalVector<int> &idx_spikes,
        const LocalVector<T> &t_spikes,
        char suffix
    );

    // Compute potentials phi_e or phi_i in observation points
    void ComputePotObsPts
    (
        LocalDenseMatrix<T, RNGT> &phi,
        const DistVector<T> &gts1,
        const DistVector<T> &gts2,
        T radius,
        int iter
    );

    // Report about start of Hebbian correction of a matrix
    void ReportHC(const char *suf);

    // Print overall progress once per reportPeriodIter iterations.
    // If we are running in background mode, refresh the file that indicates current status.
    // The method is called on master thread of master process only.
    void ReportCurrentProgress(clock_t time);

    // Save backup files "output.mat" (always) and "intermediate.mat" (if saveIntermMat == true).
    // The method is called on all threads of all processes.
    void SaveCurrentProgress(bool snapshotRequested);
    
    // Compute spike frequency for the whole network
    inline T Frequency(int num_spikes, int num, T time)
    {
        return frequencyParam * num_spikes / (T)num / time;
    }

    // Compute spike frequency for single neurons.
    // The method should be called on all ranks with all threads.
    void ComputeFrequencies(const DistVector<int> &num_spikes_single, T time, DistVector<T> &freq_single)
    {
        int localLength = num_spikes_single.localLength;
        int myThread = omp_get_thread_num();
        int startIdx = GetThreadChunkStartIdx(localLength, myThread);
        int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);
        for (int idx = startIdx; idx < endIdx; idx++)
        {
            freq_single[idx] = num_spikes_single[idx] / time;
        }
    }

    // Update number of spikes on single neurons during freqWinWidth time interval
    // The method should be called on all ranks with all threads.
    void UpdateSpikesSingle(DistVector<int> &num_spikes_single, const DistVector<bool> &recent_spikes, DistDenseMatrix<bool, RNGT> &spikes_single_array, int iter)
    {
        int rowIdx = iter % spikes_single_array.numRows;

        int localLength = num_spikes_single.localLength;
        int myThread = omp_get_thread_num();
        int startIdx = GetThreadChunkStartIdx(localLength, myThread);
        int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);
        bool *data = spikes_single_array.localData + rowIdx;

        for (int idx = startIdx; idx < endIdx; idx++)
        {
            if (data[idx * spikes_single_array.numRows])
            {
                --num_spikes_single[idx];
            }
            if (recent_spikes[idx])
            {
                ++num_spikes_single[idx];
            }
        }
        spikes_single_array.FillRowFromVector(recent_spikes, rowIdx);
    }

    // Declarations of other inlined methods defined outside the class
    // (no "inline" keyword needed in these declarations)
    T h_e_inf(T v);
    T h_i_inf(T v);
    T m_e_inf(T v);
    T m_i_inf(T v);
    T n_e_inf(T v);
    T n_i_inf(T v);
    T tau_h_e(T v);
    T tau_h_i(T v);
    T tau_n_e(T v);
    T tau_n_i(T v);

#pragma endregion

};

// Definitions of other inlined methods
#include "Kinetic/h_e_inf.h"
#include "Kinetic/h_i_inf.h"
#include "Kinetic/m_e_inf.h"
#include "Kinetic/m_i_inf.h"
#include "Kinetic/n_e_inf.h"
#include "Kinetic/n_i_inf.h"
#include "Kinetic/tau_h_e.h"
#include "Kinetic/tau_h_i.h"
#include "Kinetic/tau_n_e.h"
#include "Kinetic/tau_n_i.h"