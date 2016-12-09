function AddModelTonicParams()
%% Add parameters related to tonic current
    
    AddPanel('Model (Tonic)');

    %% Parameters of tonic current
    AddRatScalar('VTonicGABA', -55, 'mV', 'Reverse potential of tonic current');
    AddRatScalar('GTonicGABA', 0.44, 'mS / cm2', 'Conductance of tonic current');
    AddRatScalar('v_rev_i', 'VTonicGABA', 'mV', 'Reverse potential of IGABA');
    AddRatScalar('DeltaVGABA', -10, 'mV', 'Difference between reverse potential of interneuron and piramidal neurons');
    AddRatScalar('AlphaTonic', 0.5, '', 'Modulator of conductance of excitability neurons', true, 'AlphaTonic > 0');
    
    %% GTonicGABA-related parameters
    desc = {'The type of GTonicGABA model', ...
        '<b>unchecked:</b> <i>no time dependency, it''s a constant equal <b><font color="blue">GTonicGABA</font></b></i>', ...
        '<b>checked:</b> <i>the function of time which is determined as following:', ...
        'y''[t] = num_i * <b><font color="blue">Af</font></b> * (mean_Frequency_i[t - <b><font color="blue">freqDelay</font></b>] + <b><font color="blue">basicFrequency</font></b>) - <b><font color="blue">Gpump</font></b> * (y[t] - <b><font color="blue">GTonicGABAControl</font></b>)', ...
        'where y[0] = <b><font color="blue">GTonicGABA</font></b></i>'};
    AddBoolScalar('dynamicGTonicGABA', true, desc);
    
    AddRatScalar('Af', 0.0000005, 'mM', 'Rate of GABA concnetration on frequency', 'dynamicGTonicGABA');
    AddRatScalar('Gpump', 0.002, '1 / s', 'Rate of GABA pumping', 'dynamicGTonicGABA');
    AddRatScalar('GTonicGABAControl', 0.1, 'mS / cm2', 'Rating tonic conductance', 'dynamicGTonicGABA');
    AddRatScalar('basicFrequency', 5, 'kHz', 'The basic frequency', 'dynamicGTonicGABA');
    
    AddBoolScalar('enableFreqDelay', true, 'Whether to delay the frequency parameter', 'dynamicGTonicGABA');
    AddPosRatScalar('freqDelay', '50000 * dt', 'ms', 'The delay for i-network frequency', 'dynamicGTonicGABA && enableFreqDelay', 'round(freqDelay / dt) > 0');
    
end