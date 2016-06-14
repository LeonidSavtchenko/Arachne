#include "GammaSimulator.h"

// Compute syncoef_e or syncoef_i.
// The method is called in master thread of master process only.
// !! TODO: Apply multithreading and try to reuse results of previous iteration on this iteration.
template <typename T, typename RNGT>
T GammaSimulator<T, RNGT>::ComputeSynCoef
(
    T Frequency,
    int num,
    int num_spikes,
    const LocalVector<int> &idx_spikes,
    const LocalVector<T> &t_spikes,
    char suffix
)
{
    int BeginTime = 0;
    T bin = syncoefParam / Frequency;
    int width = (int)ceil(m_steps * dt / bin);  // t_final / bin = freqParam / syncoefParam * num_spikes / (T)num
    
    // Initialize auxiliary array with zeros
    int numZeros = num * width;
    if (numZeros > results.length)
    {
        // !! TODO: Resize array automatically
        printf("\n>>>>> Cannot compute syncoef_%c because the array allocated for temporary data is too small.\n"
            "      Please do one of the following:\n"
            "      1) increase HPC -> max_num_spikes_%c_factor to preallocate bigger array;\n"
            "      2) decrease the number of spikes in the %c-network in some way;\n"
            "      3) turn off Measured -> stabAnalysis;\n"
            "      4) decrease Measured -> frequencyParam;\n"
            "      5) increase Measured -> syncoefParam.\n",
            suffix, suffix, suffix);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    results.AssignZeros(numZeros);

    for (int k = BeginTime; k < num_spikes; k++)
    {
        int rowIdx = idx_spikes[k];
        int colIdx = (int)floor(t_spikes[k] / bin);
        results[rowIdx + num * colIdx] = 1;
    }

    T syncoef = 0;

    for (int nnn = BeginTime; nnn < num - 1; nnn++)
    {
        T K = 0;
        T M = 0;
        T N = 0;
        for (int sss = 0; sss < width; sss++)
        {
            K += results[nnn + num * sss] * results[nnn + 1 + num * sss];
            M += results[nnn + num * sss];
            N += results[nnn + 1 + num * sss];
        }
        T prod = M * N;
        if (prod != 0)
        {
            T term = K / sqrt(prod);
            syncoef += term;
        }
        // Remarks:
        // 1) We assume zero term if both numerator and denominator equal zero
        //    (this case is rather frequent at start of simulation;
        //    straightforward division brings NaN term that spoils resulting coefficient).
        // 2) Do not check for not NaN as (term == term) because it does not work properly under Linux compiler.
    }
    syncoef /= (num - 1);

    return syncoef;
}