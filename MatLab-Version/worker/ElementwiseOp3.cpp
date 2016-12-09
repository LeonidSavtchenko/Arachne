#include "GammaSimulator.h"

// Input vectors:        v_e_tmp, n_e_tmp, m_e_tmp, h_e_tmp, gts1_e, gts2_e, I_e, s_e_tmp, tau_r_e, tau_d_e, u_e
// Input scalars:        num_e, v_rev_e, v_rev_i, g_stoch_e, DeltaVGABA, GTonicGABA, VTonicGABA, dt, r_e, stoch_e_upd_const
// Output vectors:       m_e
// Input/output vectors: v_e, n_e, h_e, s_e, s_stoch_e
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ElementwiseOperation3()
{
    using namespace DistEnv;

    if (enableExtraCurrent_e)
    {
        extraCurrent_e.DoOneStepPart2(iter, v_e, v_e_tmp);
    }

    if (importMod_e)
    {
        modCurrent_e.DoOneStepPart2(iter, v_e, v_e_tmp);
    }

    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(v_e_tmp.localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(v_e_tmp.localLength, myThread + 1);

    for (int idx = startIdx; idx < endIdx; idx++)
    { 
        T delta = v_rev_e - v_e_tmp[idx];
        
        T v_e_inc = T(0.1) * (-67 - v_e_tmp[idx]) + 80 * pow(n_e_tmp[idx], 4) * (-100 - v_e_tmp[idx]) + 100 * pow(m_e_tmp[idx], 3) * h_e_tmp[idx] * (50 - v_e_tmp[idx]) +
            gts1_e[idx] * delta + gts2_e[idx] * (gaba.DeltaVGABA + v_rev_i - v_e_tmp[idx]) + imageDrive[idx] +
            I_e[idx] + extraCurrent_e.I[idx] + modCurrent_e.I[idx] + g_stoch_e * s_stoch_e[idx] * delta + gaba.AlphaTonic * gaba.GTonicGABA * (gaba.DeltaVGABA + gaba.VTonicGABA - v_e_tmp[idx]);
        T n_e_inc = (n_e_inf(v_e_tmp[idx]) - n_e_tmp[idx]) / tau_n_e(v_e_tmp[idx]);
        T h_e_inc = (h_e_inf(v_e_tmp[idx]) - h_e_tmp[idx]) / tau_h_e(v_e_tmp[idx]);
        T s_e_inc = T(0.5) * (1 + tanh(v_e_tmp[idx] / 4)) * (1 - s_e_tmp[idx]) / tau_r_e[idx] - s_e_tmp[idx] / tau_d_e[idx];
        
        v_e[idx] += dt * v_e_inc;
        n_e[idx] += dt * n_e_inc;
        m_e[idx] = m_e_inf(v_e[idx]);
        h_e[idx] += dt * h_e_inc;
        s_e[idx] += dt * s_e_inc;
        
        T diff = stoch_e_upd_const - u_e[idx];
        if (diff > 0)
        {
            s_stoch_e[idx] = 1;
        }
        else
        {
            s_stoch_e[idx] *= r_e;
        }
    }
}