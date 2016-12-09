function m_i_inf = m_i_inf(v)
    global m_i_v_1 m_i_v_2 m_i_a_1 m_i_a_2 m_i_b_1 m_i_b_2

    sum = v + m_i_v_1;
    alpha_m = m_i_a_1 * sum ./ (1 - exp(-sum / m_i_a_2));
    alpha_m(sum == 0) = m_i_a_1 * m_i_a_2;
    
    beta_m = m_i_b_1 * exp(-(v + m_i_v_2) / m_i_b_2);
    
    m_i_inf = alpha_m ./ (alpha_m + beta_m);
