function AddModelParams_for_UTILITY_ComputeMaxModelSize()

    AddPanel('Model');
    
    AddPosRatScalar('t_final', 1000, 'ms', 'Time simulated');
    AddPosRatScalar('dt', 0.02, 'ms', 'Time step used in the midpoint method');
    
    desc = {'Width of time window used for calculation of spikes frequencies in the following two cases:', ...
        '1) calculation of overall i-network frequency if <b><font color="blue">dynamicGTonicGABA</font></b> is checked;', ...
        '2) calculation of particular neurons frequencies if <b><font color="blue">enableSTDP</font></b> is checked.'};
    AddPosRatScalar('freqWinWidth', '5000 * dt', 'ms', desc, true, 'round(freqWinWidth / dt) > 0');
    
    %% Parameters of circular networks
    AddPosRatScalar('radius_e', 250, 'µm', 'Radius of e-network');
    AddPosRatScalar('radius_i', 200, 'µm', 'Radius of i-network');
    AddPosRatScalar('v', 0.1, 'µm / ms', 'Rate of signal propagation');
    
end