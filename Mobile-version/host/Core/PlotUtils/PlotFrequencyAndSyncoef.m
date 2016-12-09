function PlotFrequencyAndSyncoef(time, Frequency_e, Frequency_i, syncoef_e, syncoef_i)
%% Plot curves of Frequency_e, Frequency_i, syncoef_e, syncoef_i vs iteration number.
%  Plot their moving averages and bounds of stdDevFactor sample standard deviations.

    global plotFrequency plotSynCoef

    % Compute moving averages and moving sample standard deviations
    fprintf('\nComputing moving averages and moving sample standard deviations ...\n');
    if plotFrequency
        [movAvg_Frequency_e, movStdDev_Frequency_e] = GetMovAvgStdDev(Frequency_e);
        [movAvg_Frequency_i, movStdDev_Frequency_i] = GetMovAvgStdDev(Frequency_i);
    end
    if plotSynCoef
        [movAvg_syncoef_e, movStdDev_syncoef_e] = GetMovAvgStdDev(syncoef_e);
        [movAvg_syncoef_i, movStdDev_syncoef_i] = GetMovAvgStdDev(syncoef_i);
    end
    
    % Plot signals vs time,
    % for each signal plot its moving average and
    % range "moving average +- stdDevFactor * moving sample standard deviation"
    time(1) = [];   % remove time = 0
    if plotFrequency
        PlotCurve(time, Frequency_e, movAvg_Frequency_e, movStdDev_Frequency_e, 'Frequency_e', ', Hz');
        PlotCurve(time, Frequency_i, movAvg_Frequency_i, movStdDev_Frequency_i, 'Frequency_i', ', Hz');
    end
    if plotSynCoef
        PlotCurve(time, syncoef_e, movAvg_syncoef_e, movStdDev_syncoef_e, 'syncoef_e', '');
        PlotCurve(time, syncoef_i, movAvg_syncoef_i, movStdDev_syncoef_i, 'syncoef_i', '');
    end
    
end


function PlotCurve(time, signal, movAvg, movStdDev, name, unit)
%% Plot signal vs time,
%  plot its moving average and range "moving average +- stdDevFactor * moving sample standard deviation"

    global stdDevFactor
    
    figure('Name', name, 'NumberTitle', 'off');
    plot(time, signal);
    hold on
    plot(time, movAvg, 'k');
    plot(time, movAvg + stdDevFactor * movStdDev, 'r');
    plot(time, movAvg - stdDevFactor * movStdDev, 'r');
    set(gca, 'xlim', [0, time(end)]);
    name = strrep(name, '_', '\_');
    title([name, unit]);
    xlabel('time, ms');
    legend({name, '\it\mu', ['\it\mu ± ', num2str(stdDevFactor), '\it\sigma']});
    grid on
    
end