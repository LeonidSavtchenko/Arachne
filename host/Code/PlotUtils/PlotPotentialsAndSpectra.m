function PlotPotentialsAndSpectra(time, phi_e, phi_i, opRadii, opAngles)
%% Plot potentials and their power spectra in the observation points.
%  Signal Processing Toolbox is required to suppress side lobes of impulses in the spectra.

    global numFreqFactor plotPotentials plotSpectra subtractMean winType winOpts
    
    % The number of frequencies to show in the power spectra
    numTicks = length(time);
    numFreq = numFreqFactor * numTicks;

    dt = time(2) - time(1);
    
    % The number of observation points
    opNum = size(phi_e, 1);
    
    if plotPotentials
        % Plot potentials in the observation points
        titlePat = 'Potential in the point \\{ radius = %g µm; angle = %g rad \\}';
        tRange = [time(1), time(end)];
        for j = 1 : opNum
            figure('Name', sprintf('Potentials (%i)', j), 'NumberTitle', 'off');
            plot(time, phi_e(j, :), 'r');
            hold on
            plot(time, phi_i(j, :), 'b');
            plot(time, phi_e(j, :) + phi_i(j, :), 'g');
            grid on
            set(gca, 'xlim', tRange);
            xlabel('time, ms');
            ylabel('potential, mV / µm');
            title(sprintf(titlePat, opRadii(j), opAngles(j)));
            legend('e-cells', 'i-cells', 'total');
        end
    end
    
    if ~plotSpectra
        return
    end
    
    % Compute and plot power spectra in the observation points.
    % Remark: Zero frequency point is not shown by "loglog" function silently.
    lastIdx = floor(numFreq / 2) + 1;
    df = 1 / (numFreq * dt);
    freq = linspace(0, double(lastIdx - 1) * df, lastIdx);
    titlePat = 'Power density spectrum in the point \\{ radius = %g um; angle = %g rad \\}';
    fRange = [freq(1), freq(end)];

    %% Prepare weight window to suppress side lobes of impulses in the spectra
    
    % Verify if Signal Processing Toolbox is installed
    v = ver;
    toolboxName = 'Signal Processing Toolbox';
    installed = any(strcmp(toolboxName, {v.Name}));
    if installed
        win = window(winType, 2 * numTicks - 1, winOpts{:});
        win = win(end - numTicks + 1 : end)';
    else
        msg = [toolboxName ' is not installed. The rectangular window will be used.'];
        warning(msg);
        win = ones(1, numTicks);
    end
    
    %% Loop by all observation points
    for j = 1 : opNum
        if subtractMean
            % Subtract mean value from potentials to eliminate the impulse at zero frequency
            phi_e(j, :) = phi_e(j, :) - mean(phi_e(j, :));
            phi_i(j, :) = phi_i(j, :) - mean(phi_i(j, :));
        end
        spectrum_e = fft(phi_e(j, :) .* win, numFreq);
        spectrum_i = fft(phi_i(j, :) .* win, numFreq);
        spectrum_sum = spectrum_e + spectrum_i;
        powSpectrum_e = spectrum_e(1 : lastIdx) .* conj(spectrum_e(1 : lastIdx));
        powSpectrum_i = spectrum_i(1 : lastIdx) .* conj(spectrum_i(1 : lastIdx));
        powSpectrum_sum = spectrum_sum(1 : lastIdx) .* conj(spectrum_sum(1 : lastIdx));
        figure('Name', sprintf('Spectra (%i)', j), 'NumberTitle', 'off');
        loglog(freq, powSpectrum_e, 'r');
        hold on
        loglog(freq, powSpectrum_i, 'b');
        loglog(freq, powSpectrum_sum, 'g');
        grid on
        set(gca, 'xlim', fRange);
        xlabel('frequency, kHz');
        ylabel('spectrum, (mV / (µm * kHz))^{2}');
        title(sprintf(titlePat, opRadii(j), opAngles(j)));
        legend('e-cells', 'i-cells', 'total');
    end
    
end