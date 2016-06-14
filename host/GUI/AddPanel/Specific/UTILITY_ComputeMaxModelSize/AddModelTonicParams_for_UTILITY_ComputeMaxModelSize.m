function AddModelTonicParams_for_UTILITY_ComputeMaxModelSize()
%% Add parameters related to tonic current
    
    AddPanel('Model (Tonic)');

    %% GTonicGABA-related parameters
    desc = {'The type of GTonicGABA model', ...
        '<b>unchecked:</b> <i>no time dependency, it''s a constant equal <b><font color="blue">GTonicGABA</font></b></i>', ...
        '<b>checked:</b> <i>the function of time which is determined as following:', ...
        'bold', 'y''[t] = num_i * <b><font color="blue">Af</font></b> * (mean_Frequency_i[t - <b><font color="blue">freqDelay</font></b>] + <b><font color="blue">basicFrequency</font></b>) - <b><font color="blue">Gpump</font></b> * (y[t] - <b><font color="blue">GTonicGABAControl</font></b>)', ...
        'where y[0] = <b><font color="blue">GTonicGABA</font></b></i>'};
    AddBoolScalar('dynamicGTonicGABA', true, desc);
    
    AddBoolScalar('enableFreqDelay', true, 'Whether to delay the frequency parameter', 'dynamicGTonicGABA');
    AddPosRatScalar('freqDelay', '50000 * dt', 'ms', 'The delay for i-network frequency', 'dynamicGTonicGABA && enableFreqDelay', 'round(freqDelay / dt) > 0');
    
end