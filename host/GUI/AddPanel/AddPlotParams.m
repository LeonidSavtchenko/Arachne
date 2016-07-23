function AddPlotParams()

    AddPanel('Plot');
    
    AddBoolScalar('plotRastr', true, 'Plot rastergram');
    AddBoolScalar('plotQR', true, 'Plot the quality of recall coefficient', '~startFromScratch || enableSTDP');
    AddBoolScalar('plotQRPeriod', true, 'Plot 1 / Frequency_e computed from the beginning of the latest recall session');
    AddBoolScalar('plotSCM', true, 'Plot synaptic conductance matrices', '~startFromScratch || (enableSTDP && gatherSCM)');
    
    % Stabilization analysis plots parameters
    relPred = '~startFromScratch || stabAnalysis';
    AddBoolScalar('plotFrequency', true, 'Plot network frequencies', relPred);
    AddBoolScalar('plotSynCoef', true, 'Plot network synchronization coefficients', relPred);
    desc = {'The parameter used when moving averages and moving sample standard deviations are computed.', ...
            'Size of the moving window is computed as size of the signal divided by winSizeDivisor.', ...
            'The lower value of the divisor, the stronger smoothing.'};
    AddPosRatScalar('winSizeDivisor', 10, '', desc, relPred);
    AddPosRatScalar('stdDevFactor', 3, '', 'The factor used when we plot range "moving average +- stdDevFactor * moving sample standard deviation"', relPred);
    
    AddBoolScalar('plotGTonicGABA', true, 'Plot GTonicGABA', '~startFromScratch || (dynamicGTonicGABA && watchGTonicGABA)');
    
    AddBoolScalar('plotPresynVoltages', true, 'Plot presynaptic voltage vs time for selected neurons', '~startFromScratch || ~isempty(watchedCellIdx_e) || ~isempty(watchedCellIdx_i)');
    
    AddBoolScalar('plotSynCondVsTime', true, 'Plot synaptic conductance vs time for selected synapses', '~startFromScratch || ~isempty(watchedSynIdx_ee) || ~isempty(watchedSynIdx_ei) || ~isempty(watchedSynIdx_ie) || ~isempty(watchedSynIdx_ii)');
    
    AddBoolScalar('plotPotentials', true, 'Plot potentials in observation points', '~startFromScratch || (~isempty(opRadii) && ~isempty(opAngles))');    
    
    % Power spectra plots parameters
    AddBoolScalar('plotSpectra', true, 'Plot power spectra of potentials in observation points', '~startFromScratch || (~isempty(opRadii) && ~isempty(opAngles))');
    desc = {'Bin calculation of extracellular field (spectrum).', ...
            'The number of frequencies to show in the spectra:', ...
            'numFreq = <b><font color="blue">numFreqFactor</font></b> * numTicks', ...
            'where numFreq is the number of frequencies, numTicks is the number of time steps.'};
    relPred = 'plotSpectra';
    AddPosRatScalar('numFreqFactor', 10, '', desc, relPred);
    AddBoolScalar('subtractMean', true, 'Whether to subtract mean value from potentials to delete the impulse in spectra at zero frequency', relPred);
    desc = {'Type of weight window used to preprocess the potentials in order to suppress side lobes of impulses in the spectra.', ...
            'Use "@rectwin" to turn off this kind of preprocessing.', ...
            'See the list of supported window types here:', ...
            '<a href="http://www.mathworks.com/help/signal/ref/window.html">http://www.mathworks.com/help/signal/ref/window.html</a>', ...
            'If Signal Processing Toolbox is not installed, then rectangular window is used.'};
    AddStringScalar('winType', '@hamming', desc, relPred);
    AddStringVector('winOpts', '{}', '<html>Parameters of the window (see <b><font color="blue">winType</font></b> for types info)</html>', relPred);
    
end
