function AddInitCondParams()
    
    AddPanel('Initial Conditions');
    
    AddRatVector('v_e', '-70 + rand(num_e, 1) * 3', 'mV', 'Voltage of pyramidal neuron', true, 'length(v_e) == num_e');
    AddRatVector('n_e', '0 * ones(num_e, 1)', '', 'Kinetics constant', true, 'length(n_e) == num_e');
    AddRatVector('m_e', 'm_e_inf(v_e)', '', 'Kinetics constant', true, 'length(m_e) == num_e');
    AddRatVector('h_e', '0 * ones(num_e, 1)', '', 'Kinetics constant', true, 'length(h_e) == num_e');
    AddRatVector('s_e', 'zeros(num_e, 1)', '', 'Kinetics constant', true, 'length(h_e) == num_e');
    
    AddRatVector('v_i', '-70 + rand(num_i, 1) * 3', 'mV', 'Voltage of Interneurons', true, 'length(v_i) == num_i');
    AddRatVector('n_i', '0 * ones(num_i, 1)', '', 'Kinetics constant', true, 'length(n_i) == num_i');
    AddRatVector('m_i', 'm_i_inf(v_i)', '', 'Kinetics constant', true, 'length(m_i) == num_i');
    AddRatVector('h_i', '0 * ones(num_i, 1)', '', 'Kinetics constant', true, 'length(h_i) == num_i');
    AddRatVector('s_i', 'zeros(num_i, 1)', '', 'Kinetics constant', true, 'length(h_i) == num_i');
    
    AddRatVector('s_stoch_e', 'zeros(num_e, 1)', '', 'Parameter of synaptic activation of pyramidal neurons', true, 'length(s_stoch_e) == num_e');
    AddRatVector('s_stoch_i', 'zeros(num_i, 1)', '', 'Parameter of synaptic activation of inter neurons', true, 'length(s_stoch_i) == num_i');
    
end