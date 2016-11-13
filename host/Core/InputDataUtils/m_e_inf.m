function m_e_inf = m_e_inf(v)
    global m_e_v_1 m_e_v_2 m_e_a_1 m_e_a_2 m_e_b_1 m_e_b_2

    sum = v + m_e_v_1;
    alpha_m = m_e_a_1 * sum ./ (1 - exp(-sum / m_e_a_2));
    alpha_m(sum == 0) = m_e_a_1 * m_e_a_2;
    
    sum = v + m_e_v_2;
    beta_m = m_e_b_1 * sum ./ (exp(sum / m_e_b_2) - 1);
    beta_m(sum == 0) = m_e_b_1 * m_e_b_2;

    m_e_inf = alpha_m ./ (alpha_m + beta_m);
