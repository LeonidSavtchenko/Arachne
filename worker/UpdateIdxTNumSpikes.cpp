#include "GammaSimulator.h"
#include "DistEnv.h"


// Determine which and how many cells of given type (E or I) spiked in the current time step.
// Update { idx_e_spikes, t_e_spikes, num_spikes_e } or { idx_i_spikes, t_i_spikes, num_spikes_i } living on master rank.
// Update number of spikes on single neurons.
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::UpdateIdxTNumSpikes
(
    const DistVector<T> &v,
    const DistVector<T> &v_old,
    LocalVector<int> &idx_spikes,
    LocalVector<T> &t_spikes,
    DistVector<bool> &spikes_single,
    int &num_spikes,
    char letter
)
{
    using namespace DistEnv;

    // Determine which and how many cells of given type (E or I) spiked in current time step.
    // Put selected spikes on start of local portions of vectors idx_spikes_iter and t_spikes_iter.
    int num_spikes_rank = SelectSpikes(v, v_old, idx_spikes_iter, t_spikes_iter);
    
    if (enableSTDP)
    {
        spikes_single.AssignZeros();
    }

    // !! TODO: Multithreading
    #pragma omp barrier
    #pragma omp master
    {
        // Update the counter of spikes for each neuron
        if (enableSTDP)
        {
            int rankStartIdx = GetRankChunkStartIdx(spikes_single.length, myRank);
            for (int idx = 0; idx < num_spikes_rank; ++idx)
            {
                int localIdx = idx_spikes_iter[idx] - rankStartIdx;
                spikes_single[localIdx] = true;
            }
        }

        // Save data for astrocyte network
        if (enableAstro && letter == 'e')
        {
            int rankStartIdx = GetRankChunkStartIdx(v.length, myRank);
            lastNumSpikes_e = num_spikes_rank;
            for (int idx = 0; idx < lastNumSpikes_e; idx++)
            {
                lastSpikes_e[idx] = idx_spikes_iter[idx] - rankStartIdx;
            }
        }
    }
    #pragma omp barrier

    // Do incremental gathering of new spike indices from all ranks to vector idx_spikes living on master rank.
    // Do not update num_spikes because the second method Gather (see below) will do it.
    int num_spikes_copy = num_spikes;
    bool success = idx_spikes_iter.Gather(idx_spikes, num_spikes_rank, num_spikes_copy);
    
    if (myRank == MASTER_RANK)
    {
        #pragma omp master
        {
            if (!success)
            {
                // !! TODO: Resize array automatically
                printf("\n>>>>> Limit of %c-spikes exceeded. Please increase max_num_spikes_%c_factor in the \"HPC\" panel to preallocate bigger arrays.\n", letter, letter);
                cout << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        }
    }

    // Do incremental gathering of new spike times from all ranks to vector t_spikes living on master rank.
    // (No need to check for success in the second case.)
    // Update num_spikes.
    t_spikes_iter.Gather(t_spikes, num_spikes_rank, num_spikes);
}


// Given rank, analyse local portions of vectors v and v_old and determine if any spikes appear.
// If so, populate local portions of vectors idx_spikes_iter and t_spikes_iter
// (the positions with indices 0, 1, ..., num_spikes_rank - 1 are populated).
// Return number of spikes num_spikes_rank found on this rank.
template <typename T, typename RNGT>
int GammaSimulator<T, RNGT>::SelectSpikes
(
    const DistVector<T> &v,
    const DistVector<T> &v_old,
    DistVector<int> &idx_spikes_iter,
    DistVector<T> &t_spikes_iter
)
{
    using namespace DistEnv;

    int myThread = omp_get_thread_num();

    int rankStartIdx = GetRankChunkStartIdx(v.length, myRank);
    int threadStartIdx = GetThreadChunkStartIdx(v.localLength, myThread);
    int threadEndIdx = GetThreadChunkStartIdx(v.localLength, myThread + 1);

    // Array for number of spikes found on each thread
    static int *num_spikes_thread;
    
    // Barrier before master's solo
    #pragma omp barrier
    
    // Allocate memory
    #pragma omp master
    {
        if (num_spikes_thread == nullptr)
        {
            num_spikes_thread = new int[numThreads];
        }
    }
    
    // Barrier after master's solo
    #pragma omp barrier

    // Analyse data, select spikes
    int index = threadStartIdx;
    for (int idx = threadStartIdx; idx < threadEndIdx; idx++)
    {
        if (v_old[idx] < 0 && v[idx] >= 0)
        {
            // This is a spike -- select it
            idx_spikes_iter[index] = rankStartIdx + idx;
            t_spikes_iter[index] = (v[idx] * t_old - v_old[idx] * t_new) / (v[idx] - v_old[idx]);
            index++;
        }
    }
    
    // Save the number of spikes found on this thread
    num_spikes_thread[myThread] = index - threadStartIdx;
    
    // The total number of spikes found on this rank.
    // It's equal to sum of numbers of spikes found on all threads of this rank.
    // (The variable will be returned from this function.)
    static int num_spikes_rank;
    num_spikes_rank = 0;

    // Barrier before master's solo
    #pragma omp barrier
    
    #pragma omp master
    {
        // Loop by threads and copy thread-local data of found spikes to contiguous regions at start of arrays
        // idx_spikes_iter.localData and t_spikes_iter.localData.
        // (Distributed vectors idx_spikes_iter and t_spikes_iter will be gathered afterwards,
        // this is why it's necessary to provide contiguous data layout.)
        for (int threadIdx = 0; threadIdx < numThreads; threadIdx++)
        {
            int threadStartIdx_ = GetThreadChunkStartIdx(v.localLength, threadIdx);
            int num_spikes_thread_ = num_spikes_thread[threadIdx];
            // Remark: memmove is used instead of memcpy because destination and source buffers can overlap
            memmove(
                &idx_spikes_iter[num_spikes_rank],
                &idx_spikes_iter[threadStartIdx_],
                num_spikes_thread_ * sizeof(int));
            memmove(
                &t_spikes_iter[num_spikes_rank],
                &t_spikes_iter[threadStartIdx_],
                num_spikes_thread_ * sizeof(T));
            num_spikes_rank += num_spikes_thread_;
        }
    }
    
    // Barrier after master's solo
    #pragma omp barrier

    return num_spikes_rank;
}