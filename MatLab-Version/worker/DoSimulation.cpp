#include "GammaSimulator.h"
#include "DistEnv.h"
#include "OtherFileIOUtils.h"

#include "STDP/AnalyticStdpMechanism.h"
#include "STDP/TabulatedStdpMechanism.h"

#include <omp.h>

#include <ctime>    // clock_t, clock
#include <iostream> // cout, flush
#include <limits>   // quiet_NaN

using namespace std;


#define SAFE_HEBBIAN_CORRECTION(xy, ...)                            \
{                                                                   \
    auto ptr = dynamic_cast<SimpleSCM<T, RNGT>*>(g_##xy);           \
    if (ptr != nullptr)                                             \
    {                                                               \
        ReportHC(#xy);                                              \
        stdpMechanism_##xy->HebbianCorrection(ptr, __VA_ARGS__);    \
    }                                                               \
}

#define SAFE_SCM_SLICING(xy)                                                            \
{                                                                                       \
    auto ptr = dynamic_cast<DistDenseMatrix<T, RNGT>*>(g_##xy);                         \
    if (ptr != nullptr)                                                                 \
    {                                                                                   \
        ptr->CopySliceToMatrixColumn(watched_sc_##xy, iter + 1, watchedSynIdx_##xy);    \
    }                                                                                   \
}


// The wrapper of the time evolution cycle.
// The method is called on all ranks in single thread.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::DoSimulation()
{
    using namespace DistEnv;

    if (myRank == MASTER_RANK)
    {
        cout << "The main cycle reached.\n" << flush;
    }
    
    #pragma omp parallel
    {
        DoSimulationMultithreaded();
    }
}

// Report about start of Hebbian correction of a matrix
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ReportHC(const char *suf)
{
    using namespace DistEnv;

    #pragma omp barrier
    #pragma omp master
    {
        if (myRank == MASTER_RANK)
        {
            printf("    Correcting g_%s ...\n", suf);
            cout << flush;
        }
    }
    #pragma omp barrier
}

// The time evolution cycle.
// The method is called on all ranks in multiple threads.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::DoSimulationMultithreaded()
{
    using namespace DistEnv;
    
    // Timer used to estimate duration of one iteration
    static clock_t time;
    
    // Whether termination is requested by user
    static bool terminationRequested;

    // Whether dumping of a snapshot is requested by user
    static bool snapshotRequested;

    static T t_mid = numeric_limits<T>::quiet_NaN();
    
    t_new = m_steps_prev * dt;
    
    num_e_old = num_spikes_e;
    num_i_old = num_spikes_i;
    
    for (iter = m_steps_prev; iter < m_steps; )
    {
        // Barrier before master's solo
        #pragma omp barrier
        
        #pragma omp master
        {
            // Fix current time
            if (myRank == MASTER_RANK)
            {
                time = clock();
            }

            // Update modelled time
            t_old = t_new;
            t_mid = t_old + dt05;
            t_new += dt;
        }

        // Barrier after master's solo
        #pragma omp barrier
        
        // Compute 4 matrix-vector products
        if (distMatPVH)
        {
            g_ee->MatVecSubstitute(gts1_e, dist_s_e_array, delta_ee, currentIdx_e, releaseGenStates);
            g_ie->MatVecSubstitute(gts2_e, dist_s_i_array, delta_ie, currentIdx_i, releaseGenStates);
            g_ei->MatVecSubstitute(gts1_i, dist_s_e_array, delta_ei, currentIdx_e, releaseGenStates);
            g_ii->MatVecSubstitute(gts2_i, dist_s_i_array, delta_ii, currentIdx_i, releaseGenStates);
        }
        else
        {
            g_ee->MatVecSubstitute(gts1_e, local_s_e_array, delta_ee, currentIdx_e, releaseGenStates);
            g_ie->MatVecSubstitute(gts2_e, local_s_i_array, delta_ie, currentIdx_i, releaseGenStates);
            g_ei->MatVecSubstitute(gts1_i, local_s_e_array, delta_ei, currentIdx_e, releaseGenStates);
            g_ii->MatVecSubstitute(gts2_i, local_s_i_array, delta_ii, currentIdx_i, releaseGenStates);
        }
        
        // Compute deterministic external drive to E-cells
        bool firstCall = (iter == m_steps_prev);
        ComputeExternalDrive_e(t_old, firstCall);
        
        // 1st elementwise operation
        ElementwiseOperation1();
        
        // Compute deterministic external drive to I-cells
        ComputeExternalDrive_i(t_old, firstCall);
        
        // 2nd elementwise operation
        ElementwiseOperation2();
        
        // Integrate equations of astrocyte
        if (enableAstro)
        {
            astro.DoOneStepPart1(lastSpikes_e, lastNumSpikes_e);
        }
        
        // Iterate Gamma-Aminobutyric Acid mechanisms (DeltaVGABA, GTonicGABA, VTonicGABA)
        gaba.DoOneStepPart1();
        
        // Compute 4 matrix-vector products
        if (distMatPVH)
        {
            g_ee->MatVecSubstitute(gts1_e, dist_s_e_tmp_array, delta_ee, currentIdx_e, releaseGenStates);
            g_ie->MatVecSubstitute(gts2_e, dist_s_i_tmp_array, delta_ie, currentIdx_i, releaseGenStates);
            g_ei->MatVecSubstitute(gts1_i, dist_s_e_tmp_array, delta_ei, currentIdx_e, releaseGenStates);
            g_ii->MatVecSubstitute(gts2_i, dist_s_i_tmp_array, delta_ii, currentIdx_i, releaseGenStates);
        }
        else
        {
            g_ee->MatVecSubstitute(gts1_e, local_s_e_tmp_array, delta_ee, currentIdx_e, releaseGenStates);
            g_ie->MatVecSubstitute(gts2_e, local_s_i_tmp_array, delta_ie, currentIdx_i, releaseGenStates);
            g_ei->MatVecSubstitute(gts1_i, local_s_e_tmp_array, delta_ei, currentIdx_e, releaseGenStates);
            g_ii->MatVecSubstitute(gts2_i, local_s_i_tmp_array, delta_ii, currentIdx_i, releaseGenStates);
        }
        
        // Generate random vector
        u_e.FillRandom(zeroToMaxE, uGenStates);
        
        // Compute deterministic external drive to E-cells
        ComputeExternalDrive_e(t_mid);
        
        // 3rd elementwise operation
        ElementwiseOperation3();
        
        // Compute deterministic external drive to I-cells
        ComputeExternalDrive_i(t_mid);
        
        // Generate random vector
        u_i.FillRandom(zeroToMaxI, uGenStates);
       
        // 4th elementwise operation
        ElementwiseOperation4();
        
        // Integrate equations of astrocyte
        if (enableAstro)
        {
            astro.DoOneStepPart2(iter, lastSpikes_e, lastNumSpikes_e);
            astro.CorrectReleaseProbability(g_ei->rvg, iter);
        }
        
        // Iterate Gamma-Aminobutyric Acid mechanisms (DeltaVGABA, GTonicGABA, VTonicGABA)
        gaba.DoOneStepPart2(iter);
        
        // Update matrices of presynaptic voltage history
        s_e_tmp_array->FillColFromVector(s_e_tmp, currentIdx_e);
        s_i_tmp_array->FillColFromVector(s_i_tmp, currentIdx_i);
        s_e_array->FillColFromVector(s_e, currentIdx_e);
        s_i_array->FillColFromVector(s_i, currentIdx_i);
        
        // Determine which and how many e-cells spiked in the current time step.
        // Update { idx_e_spikes, t_e_spikes, num_spikes_e } living on master rank.
        UpdateIdxTNumSpikes(v_e, v_e_old, idx_e_spikes, t_e_spikes, spikes_single_e, num_spikes_e, 'e');
        
        // Determine which and how many i-cells spiked in the current time step.
        // Update { idx_i_spikes, t_i_spikes, num_spikes_i } living on master rank.
        int num_spikes_i_prev = num_spikes_i;
        UpdateIdxTNumSpikes(v_i, v_i_old, idx_i_spikes, t_i_spikes, spikes_single_i, num_spikes_i, 'i');
        
        // Update number of spikes during freqWinWidth time interval
        if (gaba.dynamicGTonicGABA && myRank == MASTER_RANK)
        {
            #pragma omp barrier
            #pragma omp master
            {
                int last = iter % spikes_i_counter.length;
                int lastWin = (iter + freqWinWidth_iter) % spikes_i_counter.length;

                sum_num_spikes_i -= spikes_i_counter[last];
                spikes_i_counter[last] = num_spikes_i - num_spikes_i_prev;
                sum_num_spikes_i += spikes_i_counter[lastWin];
            }
            #pragma omp barrier
        }
        
        if (enableSTDP)
        {
            UpdateSpikesSingle(num_spikes_single_e, spikes_single_e, dist_spikes_single_e_array, iter);
            UpdateSpikesSingle(num_spikes_single_i, spikes_single_i, dist_spikes_single_i_array, iter);
        }
        
        // Hebbian correction of matrices
        if (enableSTDP && !recallMode && ((iter + 1) % stdpPeriodIter == 0))
        {
            length_e = num_spikes_e - num_e_old;
            length_i = num_spikes_i - num_i_old;
            
            #pragma omp barrier
            #pragma omp master
            {
                MPI_Bcast(&length_e, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
                MPI_Bcast(&length_i, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
            }
            #pragma omp barrier
            
            // Vectors of recent spikes            
            t_e_spikes_recent.BroadcastData(t_e_spikes, num_e_old, length_e);
            t_i_spikes_recent.BroadcastData(t_i_spikes, num_i_old, length_i);
            idx_e_spikes_recent.BroadcastData(idx_e_spikes, num_e_old, length_e);
            idx_i_spikes_recent.BroadcastData(idx_i_spikes, num_i_old, length_i);
            
            // Spike frequency of each neuron (used in STDP equation)
            ComputeFrequencies(num_spikes_single_e, freqWinWidth, freq_single_e);
            ComputeFrequencies(num_spikes_single_i, freqWinWidth, freq_single_i);
            
            // Report about start of Hebbian correction
            #pragma omp barrier
            #pragma omp master
            {
                if (myRank == MASTER_RANK)
                {
                    printf("Starting Hebbian correction on iteration %i ...\n", iter + 1);
                    cout << flush;
                }
            }
            #pragma omp barrier
            
            // Gather neuron frequencies on all ranks
            freq_single_e.Gather(local_freq_single_e);
            freq_single_i.Gather(local_freq_single_i);
            
            // Correction of matrices
            SAFE_HEBBIAN_CORRECTION(ee, t_e_spikes_recent, idx_e_spikes_recent, length_e, local_freq_single_e, t_e_spikes_recent, idx_e_spikes_recent, length_e, freq_single_e, copy_col_e, t_new)
            SAFE_HEBBIAN_CORRECTION(ei, t_e_spikes_recent, idx_e_spikes_recent, length_e, local_freq_single_e, t_i_spikes_recent, idx_i_spikes_recent, length_i, freq_single_i, copy_col_e, t_new)
            SAFE_HEBBIAN_CORRECTION(ie, t_i_spikes_recent, idx_i_spikes_recent, length_i, local_freq_single_i, t_e_spikes_recent, idx_e_spikes_recent, length_e, freq_single_e, copy_col_i, t_new)
            SAFE_HEBBIAN_CORRECTION(ii, t_i_spikes_recent, idx_i_spikes_recent, length_i, local_freq_single_i, t_i_spikes_recent, idx_i_spikes_recent, length_i, freq_single_i, copy_col_i, t_new)
            
            // Report about end of Hebbian correction
            #pragma omp barrier
            #pragma omp master
            {
                if (myRank == MASTER_RANK)
                {
                    cout << "Done" << endl << flush;
                }
            }
            #pragma omp barrier
            
            num_e_old = num_spikes_e;
            num_i_old = num_spikes_i;
        }
        
        // Save presynaptic voltages of watched cells
        v_e.CopySliceToMatrixColumn(watched_v_e, iter + 1, watchedCellIdx_e);
        v_i.CopySliceToMatrixColumn(watched_v_i, iter + 1, watchedCellIdx_i);
        
        // Compute potentials in observation points
        ComputePotObsPts(phi_e, gts1_e, gts2_e, radius_e, iter + 1);
        ComputePotObsPts(phi_i, gts1_i, gts2_i, radius_i, iter + 1);
        
        // Save synaptic conductances of watched synapses
        SAFE_SCM_SLICING(ee)
        SAFE_SCM_SLICING(ei)
        SAFE_SCM_SLICING(ie)
        SAFE_SCM_SLICING(ii)
        
        // Barrier before master's solo
        #pragma omp barrier

        #pragma omp master
        {
            // Compute frequency for dynamic tonic current conductance
            if (gaba.dynamicGTonicGABA)
            {
                if (myRank == MASTER_RANK)
                {
                    gaba.freq_i = Frequency(sum_num_spikes_i, num_i, freqWinWidth);
                }
                MPI_Bcast(&gaba.freq_i, 1, GetMpiDataType<T>(), MASTER_RANK, MPI_COMM_WORLD);
            }
            
            // Compute parameters for image recalling
            if (imageMode && recallMode)
            {
                if (myRank == MASTER_RANK)
                {
                    int idx_recall = iter - m_steps_prev;
                    T t_recall = (idx_recall + 1) * dt;
                    recall_frequency_e[idx_recall] = Frequency(num_spikes_e - first_spike_recall + 1, num_e, t_recall) / frequencyParam;
                }
            }
            
            // Advance current indices in circular buffers
            currentIdx_e++;
            currentIdx_i++;
            if (currentIdx_e == max_delay_e)
            {
                currentIdx_e = 0;
            }
            if (currentIdx_i == max_delay_i)
            {
                currentIdx_i = 0;
            }
            
            // Determine whether termination is requested by user.
            // Determine whether dumping of a snapshot is requested by user.
            if (backgroundMode && (iter + 1) % c4cPeriodIter == 0)
            {
                // Barrier before master's solo
                MPI_Barrier(MPI_COMM_WORLD);

                if (myRank == MASTER_RANK)
                {
                    // Check if the file with name "terminate" appeared
                    terminationRequested = CheckIfTerminationRequested();

                    // Check if the file with name "snapshot" appeared
                    snapshotRequested = CheckIfSnapshotRequested();
                }
                
                // Barrier after master's solo
                MPI_Barrier(MPI_COMM_WORLD);

                MPI_Bcast(&terminationRequested, 1, MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);
                MPI_Bcast(&snapshotRequested, 1, MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);
            }
            
            // Barrier before master's solo
            MPI_Barrier(MPI_COMM_WORLD);
            
            if (myRank == MASTER_RANK)
            {
                // Calculate parameters Frequency_e, Frequency_i, syncoef_e, and syncoef_i
                if (stabAnalysis || iter == m_steps - 1 || terminationRequested || snapshotRequested)
                {
                    // Determine whether we should write the parameters to arrays or scalars
                    T &Frequency_e_ref = stabAnalysis ? Frequency_e[iter] : Frequency_e_scalar;
                    T &Frequency_i_ref = (stabAnalysis || gaba.enableFreqDelay) ? Frequency_i[iter] : Frequency_i_scalar;
                    T &syncoef_e_ref = stabAnalysis ? syncoef_e[iter] : syncoef_e_scalar;
                    T &syncoef_i_ref = stabAnalysis ? syncoef_i[iter] : syncoef_i_scalar;
                    
                    // Populate the parameters

                    Frequency_e_ref = Frequency(num_spikes_e, num_e, t_new);
                    Frequency_i_ref = Frequency(num_spikes_i, num_i, t_new);
                    
                    syncoef_e_ref = ComputeSynCoef(Frequency_e_ref, num_e, num_spikes_e, idx_e_spikes, t_e_spikes, 'e');
                    syncoef_i_ref = ComputeSynCoef(Frequency_i_ref, num_i, num_spikes_i, idx_i_spikes, t_i_spikes, 'i');
                }
                else if (gaba.enableFreqDelay)
                {
                    Frequency_i[iter] = Frequency(num_spikes_i, num_i, t_new);
                }
                
                // Print overall progress once per reportPeriodIter iterations.
                // If we are running in background mode, refresh the file that indicates current status.
                // The method is called on master thread of master process only.
                if (((iter + 1) % reportPeriodIter == 0 || iter == m_steps - 1) || snapshotRequested)
                {
                    ReportCurrentProgress(time);
                }
            }
            
            // Barrier after master's solo
            MPI_Barrier(MPI_COMM_WORLD);
        }
        
        // Barrier after master's solo
        #pragma omp barrier
        
        // If necessary, create backup files once per backupPeriodIter iterations.
        // The method is called on all threads of all processes.
        if ((saveBackupMats && ((iter + 1) % backupPeriodIter == 0 || iter == m_steps - 1)) || snapshotRequested)
        {
            SaveCurrentProgress(snapshotRequested);
        }

        // Barrier before master's solo
        #pragma omp barrier

        #pragma omp master
        {
            // If necessary, copy just saved file "output.mat" from the backup storage directory
            // to the directory "kernel-host/snapshot". Then delete the file "host-kernel/snapshot".
            if (snapshotRequested)
            {
                // Barrier before master's solo
                MPI_Barrier(MPI_COMM_WORLD);

                if (myRank == MASTER_RANK)
                {
                    CopySnapshotFromBackupStorage(!use1stBackupStorage);
                }

                // Barrier after master's solo
                MPI_Barrier(MPI_COMM_WORLD);

                snapshotRequested = false;
            }
            
            // If we are running in background mode, check if soft termination has been requested
            // (the check is done once per c4cPeriodIter iterations)
            if (backgroundMode && (iter + 1) % c4cPeriodIter == 0)
            {
                if (myRank == MASTER_RANK)
                {
                    // Check if termination has been requested by user
                    if (terminationRequested)
                    {
                        // Soft termination requested
                        cout << "HPC kernel is terminating per request of the host ...\n" << flush;

                        // Correct the total number of iterations done on all previous simulation sessions plus this one
                        // (it will be saved to "intermediate.mat" if saveIntermMat == true)
                        m_steps = iter + 1;
                    }
                }

                // Barrier after master's solo
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        
        // Barrier after master's solo
        #pragma omp barrier
        
        if (terminationRequested)
        {
            // Interrupt the main cycle
            break;
        }
        
        // Barrier before master's solo
        #pragma omp barrier

        #pragma omp master
        {
            // Update iteration number
            iter++;
        }
        
        // Barrier after master's solo
        #pragma omp barrier
    }
}