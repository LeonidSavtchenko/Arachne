function AddModelParams()

    global SclModels

    AddPanel('Model');
    
    AddIntScalar('num_e', 200, 'Number of E-cells', true, 'num_e > 1');
    AddIntScalar('num_i', 100, 'Number of I-cells', true, 'num_i > 1');
    AddPosRatScalar('t_final', 1000, 'ms', 'Time simulated');
    AddPosRatScalar('dt', 0.02, 'ms', 'Time step used in the midpoint method');
    
    desc = {'Width of time window used for calculation of spikes frequencies in the following two cases:', ...
        '1) calculation of overall i-network frequency if <b><font color="blue">dynamicGTonicGABA</font></b> is checked;', ...
        '2) calculation of particular neurons frequencies if <b><font color="blue">enableSTDP</font></b> is checked.'};
    AddPosRatScalar('freqWinWidth', '5000 * dt', 'ms', desc, true, 'round(freqWinWidth / dt) > 0');
    
    AddPosRatVector('tau_r_e', '0.1 * ones(num_e, 1)', 'ms', 'Rise time constant associated with E-cells synapses', true, 'length(tau_r_e) == num_e');
    AddPosRatVector('tau_d_e', '3 * ones(num_e, 1)', 'ms', 'Decay time constant associated with E-cells synapses', true, 'length(tau_d_e) == num_e');
    AddPosRatVector('tau_r_i', '0.3 * ones(num_i, 1)', 'ms', 'Rise time constant associated with I-cells synapses', true, 'length(tau_r_i) == num_i');
    AddPosRatVector('tau_d_i', '9 * ones(num_i, 1)', 'ms', 'Decay time constant associated with I-cells synapses', true, 'length(tau_d_i) == num_i');
    
    AddRatScalar('v_rev_e', 0, '', 'Synaptic reversal potential');
    
    AddList('sclModel', fieldnames(SclModels), SclModels.BSD, 'Synaptic connections localization model', {'bell-shaped strength and uniform density of connections', 'uniform strength and bell-shaped density of connections'});
        
    desc = {'Maximum strengths of synaptic connections', ...
            '(the density of synaptic connections is highest possible)'};
    AddRatScalar('w_ee_max_bss', 1.3, '', desc, 'sclModel == SclModels.BSS');
    AddRatScalar('w_ii_max_bss', 0.35, '', desc, 'sclModel == SclModels.BSS');
    AddRatScalar('w_ei_max_bss', 0.5, '', desc, 'sclModel == SclModels.BSS');
    AddRatScalar('w_ie_max_bss', 0.15, '', desc, 'sclModel == SclModels.BSS');
    
    desc = {'Maximum densities of synaptic connections.', ...
            'Notice that:', ...
            '1) the value > 1 gives saturation and corresponds to value = 1;', ...
            '2) the strengths of the connections are given by <b><font color="blue">g_hat_ee</font></b>, <b><font color="blue">g_hat_ii</font></b>, <b><font color="blue">g_hat_ei</font></b> and <b><font color="blue">g_hat_ie</font></b>.'};
    AddRatScalar('w_ee_max_bsd', 2, '', desc, 'sclModel == SclModels.BSD');
    AddRatScalar('w_ii_max_bsd', 0.8, '', desc, 'sclModel == SclModels.BSD');
    AddRatScalar('w_ei_max_bsd', 0.9, '', desc, 'sclModel == SclModels.BSD');
    AddRatScalar('w_ie_max_bsd', 0.3, '', desc, 'sclModel == SclModels.BSD');
    
    AddRatScalar('g_hat_ie', 'SynWeight * 0.96', '', 'Strength of synaptic connections');
    AddRatScalar('g_hat_ei', 'SynWeight * 0.80 * 5', '', 'Strength of synaptic connections');
    AddRatScalar('g_hat_ii', 'SynWeight', '', 'Strength of synaptic connections');
    AddRatScalar('g_hat_ee', '0.025 * SynWeight', '', 'Strength of synaptic connections');
    
    AddRatScalar('g_stoch_e', 0, 'mS', 'Maximum conductance of excitatory input pulses');
    AddRatScalar('f_stoch_e', 5, 'Hz', 'Frequency of Poisson train of excitatory input pulses');
    AddRatScalar('tau_d_stoch_e', 3, 'ms', 'Decay time of excitatory input pulses');
    
    AddRatScalar('g_stoch_i', 0, 'mS', 'Maximum conductance of inhibitory input pulses');
    AddRatScalar('f_stoch_i', 5, 'Hz', 'Frequency of Poisson train of inhibitory input pulses');
    AddRatScalar('tau_d_stoch_i', 3, 'ms', 'Decay time of inhibitory input pulses');
    
    AddRatScalar('r_e', 'exp(-dt / 2 / tau_d_stoch_e)', '', 'Synaptic delay of excitation');
    AddRatScalar('r_i', 'exp(-dt / 2 / tau_d_stoch_i)', '', 'Synaptic delay of inhibition');
    
    AddRatScalar('g_e', 0.1, 'mS', 'Parameter connection between i and e');
    AddRatScalar('StimInter', 0, 'pA', 'Constant depolarization of interneuron');
    
    %% Parameters of circular networks
    AddPosRatScalar('radius_e', 250, 'µm', 'Radius of e-network');
    AddPosRatScalar('radius_i', 200, 'µm', 'Radius of i-network');
    AddPosRatScalar('v', 0.1, 'µm / ms', 'Rate of signal propagation');
    
    AddRatScalar('releaseProb_ee', 0.5, '', 'Release probability within e-network', true, '(releaseProb_ee >= 0) && (releaseProb_ee <= 1)');
    AddRatScalar('releaseProb_ii', 0.5, '', 'Release probability within i-network', true, '(releaseProb_ii >= 0) && (releaseProb_ii <= 1)');
    AddRatVector('releaseProb_ei', '0.5 * ones(num_e, 1)', '', 'Release probability from e-network to i-network', true, '(length(releaseProb_ei) == num_e) && all(releaseProb_ei(:) >= 0) && all(releaseProb_ei(:) <= 1)');
    AddRatVector('releaseProb_ie', '0.5 * ones(num_i, 1)', '', 'Release probability from i-network to e-network', true, '(length(releaseProb_ie) == num_i) && all(releaseProb_ie(:) >= 0) && all(releaseProb_ie(:) <= 1)');
    
    AddPosRatScalar('sigmaDivisor_ee', 10, 'mm', 'Spatial variability of ee synapses');
    AddPosRatScalar('sigmaDivisor_ei', 12.5, 'mm', 'Spatial variability of ei synapses');
    AddPosRatScalar('sigmaDivisor_ie', 8, 'mm', 'Spatial variability of ie synapses');
    AddPosRatScalar('sigmaDivisor_ii', 11, 'mm', 'Spatial variability of ii synapses');
    
end