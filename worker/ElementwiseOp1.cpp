#include "GammaSimulator.h"

// Input vectors:        v_e, n_e, m_e, h_e, gts1, gts2, I_e, s_e, tau_r_e, tau_d_e
// Input scalars:        num_e, v_rev_e, v_rev_i, g_stoch_e, DeltaVGABA, GTonicGABA, VTonicGABA, dt05, r_e
// Output vectors:       v_e_tmp, n_e_tmp, m_e_tmp, h_e_tmp, s_e_tmp, v_e_old
// Input/output vectors: s_stoch_e
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ElementwiseOperation1()
{
    using namespace DistEnv;
    
    if (enableExtraCurrent_e)
    {
        extraCurrent_e.DoOneStepPart1(v_e);
    }

    if (importMod_e)
    {
        //!!modCurrent_e.DoOneStepPart1(v_e);
    }

    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(v_e.localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(v_e.localLength, myThread + 1);

    for (int idx = startIdx; idx < endIdx; idx++)
    {
        T delta = v_rev_e - v_e[idx];
        
        // !! TODO: Fix units inconsistency between v_e_inc and I_e
        T v_e_inc = T(0.1) * (-67 - v_e[idx]) + 80 * pow(n_e[idx], 4) * (-100 - v_e[idx]) + 100 * pow(m_e[idx], 3) * h_e[idx] * (50 - v_e[idx]) +
            gts1_e[idx] * delta + gts2_e[idx] * (gaba.DeltaVGABA + v_rev_i - v_e[idx]) + imageDrive[idx] +
            I_e[idx] + extraCurrent_e.I[idx] + modCurrent_e.I[idx] + g_stoch_e * s_stoch_e[idx] * delta + gaba.AlphaTonic * gaba.GTonicGABA * (gaba.DeltaVGABA + gaba.VTonicGABA - v_e[idx]);
        T n_e_inc = (n_e_inf(v_e[idx]) - n_e[idx]) / tau_n_e(v_e[idx]);
        T h_e_inc = (h_e_inf(v_e[idx]) - h_e[idx]) / tau_h_e(v_e[idx]);
        T s_e_inc = T(0.5) * (1 + tanh(v_e[idx] / 4)) * (1 - s_e[idx]) / tau_r_e[idx] - s_e[idx] / tau_d_e[idx];
        
        v_e_tmp[idx] = v_e[idx] + dt05 * v_e_inc;
        n_e_tmp[idx] = n_e[idx] + dt05 * n_e_inc;
        m_e_tmp[idx] = m_e_inf(v_e_tmp[idx]);
        h_e_tmp[idx] = h_e[idx] + dt05 * h_e_inc;
        s_e_tmp[idx] = s_e[idx] + dt05 * s_e_inc;
        s_stoch_e[idx] *= r_e;
        
        v_e_old[idx] = v_e[idx];
    }
}