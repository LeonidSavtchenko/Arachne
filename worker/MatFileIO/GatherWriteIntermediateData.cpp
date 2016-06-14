#include "GammaSimulator.h"
#include "SCMs/SCMFactory.h"
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


// Write data to intermediate MAT-file.
// The method is called on all ranks with any number of threads, but not inside "#pragma omp master" region.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::GatherWriteIntermediateData(const char *intermMatFile)
{
    using namespace pMat;
    using namespace DistEnv;

    if (myRank == MASTER_RANK)
    {
        #pragma omp master
        {
            // Open intermediate MAT-file for writing
            pIntermMatFile = matOpen(intermMatFile, "w");
            if (pIntermMatFile == nullptr)
            {
                printf("\n>>>>> Failed to open intermediate MAT-file for writing: %s\n", intermMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }
    }
    
    // Gather distributed vectors into local ones
    v_e.Gather(v_e_local, true);
    n_e.Gather(n_e_local, true);
    m_e.Gather(m_e_local, true);
    h_e.Gather(h_e_local, true);
    s_e.Gather(s_e_local, true);
    s_stoch_e.Gather(s_stoch_e_local, true);

    v_i.Gather(v_i_local, true);
    n_i.Gather(n_i_local, true);
    m_i.Gather(m_i_local, true);
    h_i.Gather(h_i_local, true);
    s_i.Gather(s_i_local, true);
    s_stoch_i.Gather(s_stoch_i_local, true);

    uGenStates.Gather(uGenStates_local, true);
    releaseGenStates.Gather(releaseGenStates_local, true);
    
    // Astrocyte data
    if (enableAstro)
    {
        astro.GatherWriteIntermediateData();
        lastSpikes_e.Gather(lastSpikes_local, true);
    }

    // Extra currents data
    if (enableExtraCurrent_e)
    {
        extraCurrent_e.GatherWriteIntermediateData();
    }
    if (enableExtraCurrent_i)
    {
        extraCurrent_i.GatherWriteIntermediateData();
    }

    // Gather num_spikes_single_* and spikes_single_* vectors
    if (enableSTDP)
    {
        num_spikes_single_e.Gather(num_spikes_single_e_local, true);
        num_spikes_single_i.Gather(num_spikes_single_i_local, true);

        dist_spikes_single_e_array.Gather(local_spikes_single_e_array);
        dist_spikes_single_i_array.Gather(local_spikes_single_i_array);
    }

    if (myRank == MASTER_RANK)
    {
        #pragma omp master
        {
            // Write scalars
            WriteScalar<int>(numRanks * numThreads, "totalNumThreads", MatFile::Intermediate);
            WriteScalar<int>(currentIdx_e, "currentIdx_e", MatFile::Intermediate);
            WriteScalar<int>(currentIdx_i, "currentIdx_i", MatFile::Intermediate);

            // Convert objects from LocalVector<T> to mxArray and write them out
            WriteVector<T>(v_e_local, "v_e", MatFile::Intermediate);
            WriteVector<T>(n_e_local, "n_e", MatFile::Intermediate);
            WriteVector<T>(m_e_local, "m_e", MatFile::Intermediate);
            WriteVector<T>(h_e_local, "h_e", MatFile::Intermediate);
            WriteVector<T>(s_e_local, "s_e", MatFile::Intermediate);
            WriteVector<T>(s_stoch_e_local, "s_stoch_e", MatFile::Intermediate);

            WriteVector<T>(v_i_local, "v_i", MatFile::Intermediate);
            WriteVector<T>(n_i_local, "n_i", MatFile::Intermediate);
            WriteVector<T>(m_i_local, "m_i", MatFile::Intermediate);
            WriteVector<T>(h_i_local, "h_i", MatFile::Intermediate);
            WriteVector<T>(s_i_local, "s_i", MatFile::Intermediate);
            WriteVector<T>(s_stoch_i_local, "s_stoch_i", MatFile::Intermediate);
            
            assert(!distMatPVH);

            // Convert objects from LocalDenseMatrix<T, RNGT> to mxArray and write them out
            WriteMatrix<T, RNGT>(local_s_e_array, "s_e_array", MatFile::Intermediate);
            WriteMatrix<T, RNGT>(local_s_i_array, "s_i_array", MatFile::Intermediate);
            WriteMatrix<T, RNGT>(local_s_e_tmp_array, "s_e_tmp_array", MatFile::Intermediate);
            WriteMatrix<T, RNGT>(local_s_i_tmp_array, "s_i_tmp_array", MatFile::Intermediate);
            
            if (saveIntermMat)
            {
                if (enableSTDP && !gatherSCM)
                {
                    // Convert objects from LocalDenseMatrix<T, RNGT> to mxArray and write them out
                    WriteMatrix<T, RNGT>(g_ee_local, "g_ee", MatFile::Intermediate);
                    WriteMatrix<T, RNGT>(g_ei_local, "g_ei", MatFile::Intermediate);
                    WriteMatrix<T, RNGT>(g_ie_local, "g_ie", MatFile::Intermediate);
                    WriteMatrix<T, RNGT>(g_ii_local, "g_ii", MatFile::Intermediate);
                }

                if (enableSTDP)
                {
                    WriteVector<int>(num_spikes_single_e_local, "num_spikes_single_e", MatFile::Intermediate);
                    WriteVector<int>(num_spikes_single_i_local, "num_spikes_single_i", MatFile::Intermediate);

                    WriteMatrix<bool, RNGT>(local_spikes_single_e_array, "spikes_single_e_array", MatFile::Intermediate);
                    WriteMatrix<bool, RNGT>(local_spikes_single_i_array, "spikes_single_i_array", MatFile::Intermediate);
                }
            }
            
            // Gamma-Aminobutyric Acid mechanisms
            gaba.GatherWriteIntermediateData();

            if (gaba.dynamicGTonicGABA)
            {
                WriteVector<int>(spikes_i_counter, "spikes_i_counter", MatFile::Intermediate);
                WriteScalar<int>(sum_num_spikes_i, "sum_num_spikes_i", MatFile::Intermediate);
            }
            
            if (enableAstro)
            {
                WriteScalar<int>(lastNumSpikes_e, "lastNumSpikes_e", MatFile::Intermediate);
                WriteVector<int>(lastSpikes_local, "lastSpikes", MatFile::Intermediate);
            }
            
            // Little hack to make it possible to serialize objects of type std::mt19937 or std::mt19937_64 to the MAT-file
            // !! TODO: Investigate why Linux compiler does not allow commented code while MSVS compiler allows
            /*
            LocalVector<uint8_t> uGenStates_local2 = uGenStates_local.CastReshape<uint8_t>();
            */
            uint8_t *typeMarker = nullptr;
            LocalVector<uint8_t> uGenStates_local2 = uGenStates_local.CastReshape(typeMarker);
            LocalVector<uint8_t> releaseGenStates_local2 = releaseGenStates_local.CastReshape(typeMarker);

            WriteVector<uint8_t>(uGenStates_local2, "uGenStates", MatFile::Intermediate);
            WriteVector<uint8_t>(releaseGenStates_local2, "releaseGenStates", MatFile::Intermediate);

            // Close intermediate MAT-file
            int status = matClose(pIntermMatFile);
            if (status != 0)
            {
                printf("\n>>>>> Failed to close intermediate MAT-file after writing: %s\n", intermMatFile);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }
    }
}