function ReadAndVisualizeResults()
%% Read output MAT-file from disk and visualize results of simulation.
%  This function is called after the file is downloaded or copied
%  from the HPC kernel output directory to the host directory.

    global plotRastr plotQR plotQRPeriod plotSCM plotFrequency plotSynCoef
    global plotGTonicGABA plotPresynVoltages plotSynCondVsTime
    global plotPotentials plotSpectra
    global plotCaStyle plotActParamStyle plotReleaseProbStyle 
    global plotCaColormap
    global plotI_eStyle plotI_iStyle
        
    % Read output file
    load('output.mat');
    
    % Close all windows
    close all;  
    
    if watchedCellNum_e == 0
        watchedCellIdx_e = [];
    end
    if watchedCellNum_i == 0
        watchedCellIdx_i = [];
    end
    if ~enableAstro || watchedAstroNum == 0
        watchedAstroIdx = [];
    end
    if watchedSynNum_ee == 0
        watchedSynIdx_ee = [];
    end
    if watchedSynNum_ei == 0
        watchedSynIdx_ei = [];
    end
    if watchedSynNum_ie == 0
        watchedSynIdx_ie = [];
    end
    if watchedSynNum_ii == 0
        watchedSynIdx_ii = [];
    end
    
    %% Show matrices of synaptic conductance
    if plotSCM && gatherSCM
        figure('Name', 'SCMs', 'NumberTitle', 'off');
        PlotSCM(1, g_ee, 'g_{ee}');
        PlotSCM(2, g_ii, 'g_{ii}');
        PlotSCM(3, g_ie, 'g_{ie}');
        PlotSCM(4, g_ei, 'g_{ei}');
    end
    
    %% Plot rastergram
    time = linspace(0, double(m_steps) * dt, m_steps + 1);
    if ~imageMode
        numSession = 0;
        firstStepSession = [];
        modeSession = [];
        imageSession = [];
    else
        firstStepSession = [firstStepSession; m_steps + 1]; %#ok<NODEF>
    end
    if plotRastr
        PlotRastergram(num_e, num_i, time(end), idx_e_spikes, t_e_spikes, idx_i_spikes, t_i_spikes, watchedCellIdx_e, watchedCellIdx_i, watchedAstroIdx, dt, numSession, firstStepSession, modeSession, imageMode, imageSession);
    end
    
    %% Plot presynaptic voltage vs time for selected neurons
    if plotPresynVoltages
        if watchedCellNum_e ~= 0
            PlotPresynapticVoltages(time, watched_v_e, 'e', watchedCellIdx_e);
        end
        if watchedCellNum_i ~= 0
            PlotPresynapticVoltages(time, watched_v_i, 'i', watchedCellIdx_i);
        end
    end
    
    %% Plot synaptic conductance vs time for selected synapses
    if plotSynCondVsTime
        if watchedSynNum_ee ~= 0
            PlotSynCondVsTime(time, watched_sc_ee, 'ee', watchedSynIdx_ee);
        end
        if watchedSynNum_ei ~= 0
            PlotSynCondVsTime(time, watched_sc_ei, 'ei', watchedSynIdx_ei);
        end
        if watchedSynNum_ie ~= 0
            PlotSynCondVsTime(time, watched_sc_ie, 'ie', watchedSynIdx_ie);
        end
        if watchedSynNum_ii ~= 0
            PlotSynCondVsTime(time, watched_sc_ii, 'ii', watchedSynIdx_ii);
        end
    end
    
    %% Plot potentials and their power spectra in the observation points
    if (plotPotentials || plotSpectra) && opNum ~= 0
        PlotPotentialsAndSpectra(time, phi_e, phi_i, opRadii, opAngles);
    end
    
    %% Plot GTonicGABA vs time
    if plotGTonicGABA && dynamicGTonicGABA && watchGTonicGABA
        PlotGTonicGABA(time, GTonicGABA);
    end
    
    %% Plot the quality of image recall vs time
    if (plotQR || plotQRPeriod) && imageMode && recallMode
        % Find the last memorization session
        for i = numSession : -1 : 1
            if modeSession(i) == 0
                num = i;
                break
            end
        end
        memorImage = imageSession(:, num);
        m_steps_recall = m_steps + 1 - firstStepSession(numSession);
        PlotQualityOfRecall(idx_e_spikes, t_e_spikes, first_spike_recall, m_steps, m_steps_recall, dt, memorImage, recall_frequency_e);
    end
    
    %% Plot Frequency_* and syncoef_* curves or simply print the values
    if stabAnalysis
        % The parameters are vectors
        if plotFrequency || plotSynCoef
            PlotFrequencyAndSyncoef(time, Frequency_e, Frequency_i, syncoef_e, syncoef_i);
        end
    else
        % The parameters are scalars calculated just once after the last iteration
        fprintf('\n');
        disp(['Frequency_e = ', num2str(Frequency_e), ' Hz']);
        % Print Frequency_i value
        if enableFreqDelay
            freq_i = Frequency_i(end); %#ok<COLND>
        else
            freq_i = Frequency_i;
        end
        disp(['Frequency_i = ', num2str(freq_i), ' Hz']);
        disp(['syncoef_e = ', num2str(syncoef_e)]);
        disp(['syncoef_i = ', num2str(syncoef_i)]);
    end
    
    %% Plot curves/maps of the astrocyte-related parameters vs time
    if enableAstro
        if watchedAstroNum > 0
            % Plot the curve of Ca vs time for selected astrocytes
            PlotWatchedCurves(time, watchedAstroIdx, watchedAstroCa, plotCaStyle, 'Calcium (a-%s)', 'Calcium', 'Astrocyte');
        
            % Plot the curve of activation parameter "y" vs time for selected astrocytes
            PlotWatchedCurves(time, watchedAstroIdx, watchedAstroY, plotActParamStyle, 'Activation (a-%s)', 'Activation Parameter "y"', 'Astrocyte');
        
            % Plot the curve of release probability "e->i" vs time for selected e-neurons
            PlotWatchedCurves(time, watchedAstroIdx, watchedProb, plotReleaseProbStyle, 'Probability "e->i" (e-%s)', 'Release Probability "e->i"', 'e-cell');
        end
        
        % Plot calcium colormap
        if plotCaColormap && gatherCaColormap
            PlotCaColormap(time, dt, CaColormap, watchedAstroIdx);
        end
    end

    if enableExtraCurrent_e && watchedExtraCurrentNum_e > 0
        % Plot the curve of ExtraCurrent vs time for selected e-neurons
        PlotWatchedCurves(time, watchedExtraCurrentIdx_e, watchedExtraCurrentI_e, plotI_eStyle, 'ExtraCurrent (e-%s)', 'ExtraCurrent\_e', 'e-cell');
    end
    if enableExtraCurrent_i && watchedExtraCurrentNum_i > 0
        % Plot the curve of ExtraCurrent vs time for selected e-neurons
        PlotWatchedCurves(time, watchedExtraCurrentIdx_i, watchedExtraCurrentI_i, plotI_iStyle, 'ExtraCurrent (i-%s)', 'ExtraCurrent\_i', 'i-cell');
    end
    
end