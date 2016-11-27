#include "GammaSimulator.h"

// Input vectors:        v_i_tmp, n_i_tmp, m_i_tmp, h_i_tmp, gts1_i, gts2_i, I_i, s_i_tmp, tau_r_i, tau_d_i, u_i
// Input scalars:        num_i, v_rev_e, v_rev_i, g_e, StimInter, g_stoch_i, GTonicGABA, VTonicGABA, dt, r_i, stoch_i_upd_const
// Output vectors:       m_i
// Input/output vectors: v_i, n_i, h_i, s_i, s_stoch_i
template <typename T, typename RNGT>
void GammaSimulator<T, RNGT>::ElementwiseOperation4()
{
    using namespace DistEnv;

    if (enableExtraCurrent_i)
    {
        extraCurrent_i.DoOneStepPart2(iter, v_i, v_i_tmp);
    }

    if (importMod_i)
    {
        //!!modCurrent_i.DoOneStepPart2(iter, v_i, v_i_tmp);
    }

    int myThread = omp_get_thread_num();
    int startIdx = GetThreadChunkStartIdx(v_i_tmp.localLength, myThread);
    int endIdx = GetThreadChunkStartIdx(v_i_tmp.localLength, myThread + 1);

    for (int idx = startIdx; idx < endIdx; idx++)
    {
        T delta = v_rev_e - v_i_tmp[idx];
        
        T v_i_inc = T(0.1) * (-65 - v_i_tmp[idx]) + 9 * pow(n_i_tmp[idx], 4) * (-90 - v_i_tmp[idx]) + 35 * pow(m_i_tmp[idx], 3) * h_i_tmp[idx] * (55 - v_i_tmp[idx]) +
            g_e * gts1_i[idx] * delta + gts2_i[idx] * (v_rev_i - v_i_tmp[idx]) +
            StimInter * I_i[idx] + extraCurrent_i.I[idx] + modCurrent_i.I[idx] + g_stoch_i * s_stoch_i[idx] * delta + gaba.GTonicGABA * (gaba.VTonicGABA - v_i_tmp[idx]);
        T n_i_inc = (n_i_inf(v_i_tmp[idx]) - n_i_tmp[idx]) / tau_n_i(v_i_tmp[idx]);
        T h_i_inc = (h_i_inf(v_i_tmp[idx]) - h_i_tmp[idx]) / tau_h_i(v_i_tmp[idx]);
        T s_i_inc = T(0.5) * (1 + tanh(v_i_tmp[idx] / 4)) * (1 - s_i_tmp[idx]) / tau_r_i[idx] - s_i_tmp[idx] / tau_d_i[idx];
        
        v_i[idx] += dt * v_i_inc;
        n_i[idx] += dt * n_i_inc;
        m_i[idx] = m_i_inf(v_i[idx]);
        h_i[idx] += dt * h_i_inc;
        s_i[idx] += dt * s_i_inc;
        
        T diff = stoch_i_upd_const - u_i[idx];
        if (diff > 0)
        {
            s_stoch_i[idx] = 1;
        }
        else
        {
            s_stoch_i[idx] *= r_i;
        }
    }
}