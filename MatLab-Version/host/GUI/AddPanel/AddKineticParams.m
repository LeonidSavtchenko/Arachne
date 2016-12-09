function AddKineticParams()

    AddPanel('Kinetic');
    
    desc = 'Defines the current in neurons';
    
    AddRatScalar('h_i_v_1', 58, '', desc);
    AddRatScalar('m_i_v_1', 35, '', desc);
    AddRatScalar('n_i_v_1', 34, '', desc);
    AddRatScalar('h_e_v_1', 50, '', desc);
    AddRatScalar('m_e_v_1', 54, '', desc);
    AddRatScalar('n_e_v_1', 52, '', desc);
    
    AddRatScalar('h_i_v_2', 28, '', desc);
    AddRatScalar('m_i_v_2', 60, '', desc);
    AddRatScalar('n_i_v_2', 44, '', desc);
    AddRatScalar('h_e_v_2', 27, '', desc);
    AddRatScalar('m_e_v_2', 27, '', desc);
    AddRatScalar('n_e_v_2', 57, '', desc);
    
    AddRatScalar('h_i_a_1', 0.07, '', desc);
    AddRatScalar('m_i_a_1', 0.1, '', desc);
    AddRatScalar('n_i_a_1', -0.01, '', desc);
    AddRatScalar('h_e_a_1', 0.128, '', desc);
    AddRatScalar('m_e_a_1', 0.32, '', desc);
    AddRatScalar('n_e_a_1', 0.032, '', desc); 
    
    AddRatScalar('h_i_a_2', 20, '', desc);
    AddRatScalar('m_i_a_2', 10, '', desc);
    AddRatScalar('n_i_a_2', -0.1, '', desc);
    AddRatScalar('h_e_a_2', 18, '', desc);
    AddRatScalar('m_e_a_2', 4, '', desc);
    AddRatScalar('n_e_a_2', 5, '', desc); 
    
    AddRatScalar('h_i_b_1', 1, '', desc);
    AddRatScalar('m_i_b_1', 4, '', desc);
    AddRatScalar('n_i_b_1', 0.125, '', desc);
    AddRatScalar('h_e_b_1', 4, '', desc);
    AddRatScalar('m_e_b_1', 0.28, '', desc);
    AddRatScalar('n_e_b_1', 0.5, '', desc); 
    
    AddRatScalar('h_i_b_2', -0.1, '', desc);
    AddRatScalar('m_i_b_2', 18, '', desc);
    AddRatScalar('n_i_b_2', 80, '', desc);
    AddRatScalar('h_e_b_2', 5, '', desc);
    AddRatScalar('m_e_b_2', 5, '', desc);
    AddRatScalar('n_e_b_2', 40, '', desc); 
    AddRatScalar('phi', 5, '', desc);
   