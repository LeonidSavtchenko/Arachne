function AddMeasuredParams()

    AddPanel('Measured');
    
    AddPosRatVector('opRadii', '[1234]', 'µm', 'Radii of the observation points in polar coordinate system', true, 'length(opRadii) == length(opAngles)');
    AddRatVector('opAngles', '[2.5]', 'rad', 'Angles of the observation points in polar coordinate system', true, 'length(opRadii) == length(opAngles)');
    AddRatScalar('electrolCond', 100, 'S', 'The electolytic conductance', '~isempty(opRadii) && ~isempty(opAngles)');
    
    desc = {'Stabilization analysis mode', ...
            '<b>unchecked:</b> <i>compute Frequency_e, Frequency_i, syncoef_e, syncoef_i just once at the end of the simulation;</i>', ...
            '<b>checked:</b> <i>compute Frequency_e, Frequency_i, syncoef_e, syncoef_i on each iteration</i>', ...
            '(this affects performance and does not follow the original Matlab code).'};
    AddBoolScalar('stabAnalysis', true, desc);
    AddBoolScalar('gatherSCM', true, 'Show matrices of synaptic conductance at end of simulation');
    AddBoolScalar('watchGTonicGABA', true, 'Whether to collect data to plot GTonicGABA vs time', 'dynamicGTonicGABA');
    
    AddPosIntVector('watchedCellIdx_e', '[150]', 'Array of indexes of e-cells to watch presynaptic voltage of <br>(the curves of voltage vs time will be shown)');
    AddPosIntVector('watchedCellIdx_i', '[25]', 'Array of indexes of i-cells to watch presynaptic voltage of <br>(the curves of voltage vs time will be shown)');
    
    AddPosIntMatrix('watchedSynIdx_ee', 'zeros(0, 2)', 'Array of indexes of ee-synapses to watch synaptic conductance for');
    AddPosIntMatrix('watchedSynIdx_ei', '[30, 52]', 'Array of indexes of ei-synapses to watch synaptic conductance for');
    AddPosIntMatrix('watchedSynIdx_ie', 'zeros(0, 2)', 'Array of indexes of ie-synapses to watch synaptic conductance for');
    AddPosIntMatrix('watchedSynIdx_ii', 'zeros(0, 2)', 'Array of indexes of ii-synapses to watch synaptic conductance for');

    AddRatScalar('frequencyParam', 2000, 'Hz', 'Network frequency');
    AddRatScalar('syncoefParam', 500, '', 'Network synchronization');
    
end