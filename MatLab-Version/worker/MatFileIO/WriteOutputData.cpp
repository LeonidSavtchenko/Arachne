#include "GammaSimulator.h"
#include "Containers/LocalVector.h"
#include "MatFileIOUtils.h"
#include "OtherFileIOUtils.h"
#include "DistEnv.h"
#include "SCMs/ScmType.h"
#include "SclModel.h"

#include <stdio.h>  // printf
#include <stdint.h> // uint8_t, uint64_t

#include <iostream> // cout, flush
#include <random>   // mt19937, mt19937_64

using namespace std;


// Write data to output MAT-file.
// The method is called just on master thread of master rank.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::WriteOutputData(const char *outMatFile)
{
    using namespace pMat;

    // Open output MAT-file for writing
    pOutMatFile = matOpen(outMatFile, "w");
    if (pOutMatFile == nullptr)
    {
        printf("\n>>>>> Failed to open output MAT-file for writing: %s\n", outMatFile);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // Transfer data from the input MAT-file to the output MAT-file
    WriteTransitMxArrays();
    
    // Write scalars
    WriteScalar<int>(num_e, "num_e");
    WriteScalar<int>(num_i, "num_i");
    int num_steps;
    if (iter < m_steps)
    {
        // Saving BEFORE or ON the last iteration
        num_steps = iter + 1;
    }
    else
    {
        // Saving AFTER the last iteration
        num_steps = m_steps;
    }
    WriteScalar<int>(num_steps, "m_steps");
    WriteScalar<T>(dt, "dt");
    WriteScalar<int>(num_spikes_e, "num_spikes_e");
    WriteScalar<int>(num_spikes_i, "num_spikes_i");
    WriteScalar<bool>(stabAnalysis, "stabAnalysis");
    WriteScalar<bool>(gatherSCM, "gatherSCM");

    // Adjust 0-based indices to 1-based in Matlab
    for (int i = 0; i < idx_e_spikes.length; ++i)
    {
        ++idx_e_spikes[i];
    }
    for (int i = 0; i < idx_i_spikes.length; ++i)
    {
        ++idx_i_spikes[i];
    }

    // Convert objects from LocalVector<T> to mxArray and write them out
    WriteCutVector<int>(idx_e_spikes, "idx_e_spikes", num_spikes_e);
    WriteCutVector<int>(idx_i_spikes, "idx_i_spikes", num_spikes_i);
    WriteCutVector<T>(t_e_spikes, "t_e_spikes", num_spikes_e);
    WriteCutVector<T>(t_i_spikes, "t_i_spikes", num_spikes_i);

    if (stabAnalysis)
    {
        // The parameters were calculated on each iteration

        WriteCutVector<T>(Frequency_e, "Frequency_e", num_steps);
        WriteCutVector<T>(Frequency_i, "Frequency_i", num_steps);
        WriteCutVector<T>(syncoef_e, "syncoef_e", num_steps);
        WriteCutVector<T>(syncoef_i, "syncoef_i", num_steps);
    }
    else
    {
        if (gaba.enableFreqDelay)
        {
            WriteCutVector<T>(Frequency_i, "Frequency_i", num_steps);
        }
        else
        {
            WriteScalar<T>(Frequency_i_scalar, "Frequency_i");
        }
        // The parameters were calculated only once after the last iteration
        WriteScalar<T>(Frequency_e_scalar, "Frequency_e");
        WriteScalar<T>(syncoef_e_scalar, "syncoef_e");
        WriteScalar<T>(syncoef_i_scalar, "syncoef_i");
    }
    
    if (gatherSCM)
    {
        // Convert objects from LocalDenseMatrix<T, RNGT> to mxArray and write them out
        WriteMatrix<T, RNGT>(g_ee_local, "g_ee", MatFile::Output);
        WriteMatrix<T, RNGT>(g_ei_local, "g_ei", MatFile::Output);
        WriteMatrix<T, RNGT>(g_ie_local, "g_ie", MatFile::Output);
        WriteMatrix<T, RNGT>(g_ii_local, "g_ii", MatFile::Output);
    }

    // Presynaptic voltages of watched cells
#define SaveHelper(x)                                                                               \
    WriteScalar<int>(watchedCellNum_##x, "watchedCellNum_" #x);                                     \
    if (watchedCellNum_##x != 0)                                                                    \
    {                                                                                               \
        WriteMxArray(watchedCellIdxOneBased_##x, "watchedCellIdx_" #x);                             \
        WriteCutMatrix<T, RNGT>(watched_v_##x, "watched_v_" #x, num_steps + 1, MatFile::Output);    \
    }

    SaveHelper(e)
    SaveHelper(i)

#undef SaveHelper

    // Synaptic conductances of watched synapses
#define SaveHelper(xy)                                                                                  \
    WriteScalar<int>(watchedSynNum_##xy, "watchedSynNum_" #xy);                                         \
    if (watchedSynNum_##xy != 0)                                                                        \
    {                                                                                                   \
        WriteMxArray(watchedSynIdxOneBased_##xy, "watchedSynIdx_" #xy);                                 \
        WriteCutMatrix<T, RNGT>(watched_sc_##xy, "watched_sc_" #xy, num_steps + 1, MatFile::Output);    \
    }

    SaveHelper(ee)
    SaveHelper(ei)
    SaveHelper(ie)
    SaveHelper(ii)

#undef SaveHelper

    // Potentials in observation points
    WriteScalar<int>(opNum, "opNum");
    if (opNum != 0)
    {
        WriteVector<T>(opRadii, "opRadii");
        WriteVector<T>(opAngles, "opAngles");

        WriteCutMatrix<T, RNGT>(phi_e, "phi_e", num_steps + 1, MatFile::Output);
        WriteCutMatrix<T, RNGT>(phi_i, "phi_i", num_steps + 1, MatFile::Output);
    }
    
    // Gamma-Aminobutyric Acid mechanisms (DeltaVGABA, GTonicGABA, VTonicGABA)
    gaba.WriteOutputData(num_steps);
    
    // Image memorization
    WriteScalar<bool>(imageMode, "imageMode");
    WriteScalar<bool>(recallMode, "recallMode");
    if (imageMode)
    {
        WriteScalar<int>(numSession, "numSession");
        WriteVector<int>(firstStepSession, "firstStepSession");
        WriteVector<int>(modeSession, "modeSession");
        WriteMatrix<bool, RNGT>(imageSession, "imageSession", MatFile::Output);
    
        if (recallMode)
        {
            int num_steps_recall = num_steps - m_steps_prev;
            WriteScalar<int>(first_spike_recall, "first_spike_recall");
            WriteCutVector<T>(recall_frequency_e, "recall_frequency_e", num_steps_recall);
        }
    }
    
    // Astrocyte network
    WriteScalar<bool>(enableAstro, "enableAstro");
    if (enableAstro)
    {
        astro.WriteOutputData(num_steps);
    }

    // Extra currents
    WriteScalar<bool>(enableExtraCurrent_e, "enableExtraCurrent_e");
    if (enableExtraCurrent_e)
    {
        extraCurrent_e.WriteOutputData(num_steps);
    }
    WriteScalar<bool>(enableExtraCurrent_i, "enableExtraCurrent_i");
    if (enableExtraCurrent_i)
    {
        extraCurrent_i.WriteOutputData(num_steps);
    }
    
    // Mod currents
    WriteScalar<bool>(importMod_e, "importMod_e");
    if (importMod_e)
    {
        modCurrent_e.WriteOutputData(num_steps);
    }
    WriteScalar<bool>(importMod_i, "importMod_i");
    if (importMod_i)
    {
        modCurrent_i.WriteOutputData(num_steps);
    }

    // Close output MAT-file
    int status = matClose(pOutMatFile);
    if (status != 0)
    {
        printf("\n>>>>> Failed to close output MAT-file: %s\n", outMatFile);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
}

// Transfer data from the input MAT-file to the output MAT-file.
// The method is called just on master rank.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::WriteTransitMxArrays()
{
    WriteMxArray(outFileName, "outFileName");
    WriteMxArray(saveInput2Output, "saveInput2Output");

    // watchedCellIdxOneBased_* and watchedSynIdxOneBased_** are saved in WriteOutputData
}
