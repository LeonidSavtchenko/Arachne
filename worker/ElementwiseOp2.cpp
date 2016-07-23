#include "GammaSimulator.h"

// Input vectors:        v_i, n_i, m_i, h_i, gts1_i, gts2_i, I_i, s_i, tau_r_i, tau_d_i
// Input scalars:        num_i, v_rev_e, v_rev_i, g_e, StimInter, g_stoch_i, GTonicGABA, VTonicGABA, dt05, r_i
// Output vectors:       v_i_tmp, n_i_tmp, m_i_tmp, h_i_tmp, s_i_tmp, v_i_old
// Input/output vectors: s_stoch_i
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ElementwiseOperation2()
{
    using namespace DistEnv;
    
    if (enableExtraCurrent_i)
    {
        extraCurrent_i.DoOneStepPart1(v_i);
    }

    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(v_i.localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(v_i.localLength, myThread + 1);

    for (int idx = startIdx; idx < endIdx; idx++)
    {
        T delta = v_rev_e - v_i[idx];
        
        T v_i_inc = T(0.1) * (-65 - v_i[idx]) + 9 * pow(n_i[idx], 4) * (-90 - v_i[idx]) + 35 * pow(m_i[idx], 3) * h_i[idx] * (55 - v_i[idx]) +
            g_e * gts1_i[idx] * delta + gts2_i[idx] * (v_rev_i - v_i[idx]) +
            StimInter * I_i[idx] + extraCurrent_i.I[idx] + g_stoch_i * s_stoch_i[idx] * delta + gaba.GTonicGABA * (gaba.VTonicGABA - v_i[idx]);
        T n_i_inc = (n_i_inf(v_i[idx]) - n_i[idx]) / tau_n_i(v_i[idx]);
        T h_i_inc = (h_i_inf(v_i[idx]) - h_i[idx]) / tau_h_i(v_i[idx]);
        T s_i_inc = T(0.5) * (1 + tanh(v_i[idx] / 4)) * (1 - s_i[idx]) / tau_r_i[idx] - s_i[idx] / tau_d_i[idx];
        
        v_i_tmp[idx] = v_i[idx] + dt05 * v_i_inc;
        n_i_tmp[idx] = n_i[idx] + dt05 * n_i_inc;
        m_i_tmp[idx] = m_i_inf(v_i_tmp[idx]);
        h_i_tmp[idx] = h_i[idx] + dt05 * h_i_inc;
        s_i_tmp[idx] = s_i[idx] + dt05 * s_i_inc;
        s_stoch_i[idx] *= r_i;
        
        v_i_old[idx] = v_i[idx];
    }
}