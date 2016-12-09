function PlotStdpModels(varargin)
%% Common entry point for desktop and mobile versions of Matlab

    % Clear all variables, but do not remove breakpoints
    clearvars -except varargin;
    clearvars -global -except mobileMode;
    
    close all;
    
    clc;
    
    global guiType GuiTypes
    global pushbutton_OK_Delegate

    close all

    BasicParams(varargin{:});
    
    guiType = GuiTypes.UtilityPlotStdpModels;
  
    pushbutton_OK_Delegate = @SCRIPT_Core;
 
    PrepareAndShowGUI(); 
    
end

function SCRIPT_Core()

    global windowedSTDP
    
    % The number of points to plot curves
    numPts = 100001;
    
    DoPlotStdpModels(numPts);
    
    if windowedSTDP
        DoPlotEpsilonSmoothing(numPts);
    end
    
end

%%
function DoPlotStdpModels(numPts)
    
    global dt stdpPeriodIter
    global importSTDP
    global stdpParams                                       % for importSTDP = false
    global stdpFile_ee stdpFile_ei stdpFile_ie stdpFile_ii  % for importSTDP = true
        
    stdpXYmodel = cell(4, 1);
    if importSTDP
        stdpMatrix_ee = dlmread(stdpFile_ee);
        stdpMatrix_ei = dlmread(stdpFile_ei);
        stdpMatrix_ie = dlmread(stdpFile_ie);
        stdpMatrix_ii = dlmread(stdpFile_ii);
        
        stdpXYmodel{1} = stdpMatrix_ee;
        stdpXYmodel{2} = stdpMatrix_ei;
        stdpXYmodel{3} = stdpMatrix_ii;
        stdpXYmodel{4} = stdpMatrix_ie;
    end
   
    tmax = 1.2 * double(stdpPeriodIter) * dt;
    deltast = linspace(-tmax, tmax, numPts);
    
    suffix = {'ee', 'ei', 'ie', 'ii'};
    color = {'r', 'g', 'b', 'm'};
    numMods = 4;
    
    figure('Name', 'STDP models', 'NumberTitle', 'off');
    
    yMin = inf;
    yMax = -inf;
    for i = 1 : numMods
        if ~importSTDP
            factors = AnalyticAtomicCorrection(stdpParams(:, i), deltast);
            
            % Compute 0.1 and 0.9 quantiles.
            % Statistics Toolbox is required.
            qnts = quantile(factors, [0.1, 0.9]);
        
            yMin = min(yMin, qnts(1));
            yMax = max(yMax, qnts(2));
        else
            factors = TabulatedAtomicCorrection(stdpXYmodel{i}, deltast);
            
            yMin = min(yMin, min(factors));
            yMax = max(yMax, max(factors));
        end
        
        plot(deltast, factors, color{i});
        hold on
    end
    
    centre = (yMin + yMax) / 2;
    halfRange = yMax - yMin;
    
    yLim = centre + halfRange * [-1, 1];
    
    boundary = double(stdpPeriodIter) * dt * [1, 1];
    plot(-boundary, yLim, 'k--');
    plot(boundary, yLim, 'k--');
    
    set(gca, 'xLim', [deltast(1), deltast(end)]);
    set(gca, 'yLim', yLim);
    grid on
    xlabel('{\Delta}{\itt}, ms');
    ylabel('{\Delta}{\itg_{ij}} / {\itg_{ij}}');
    
    leg = cell(numMods + 1, 1);
    for i = 1 : numMods
        leg{i} = sprintf('{\\itg}_{%s}', suffix{i});
    end
    leg{numMods + 1} = '±STDP period';
    
    legend(leg);
    
    title('Factors in STDP models');

end

% Keep it in sync with the function of the same name in worker/STDP/AnalyticStdpMechanism.h
function factors = AnalyticAtomicCorrection(stdpData, deltast)
    
    Aplus      = stdpData(1);
    S          = stdpData(2);
    epsilon    = stdpData(3);
    Aminus     = stdpData(4);
    tau2       = stdpData(5);
    S2         = stdpData(6);   
    AC         = stdpData(7);
    TimeCosExp = stdpData(8);
    TimeCos    = stdpData(9);
    AS         = stdpData(10);
    TimeSinExp = stdpData(11);
    TimeAbs    = stdpData(12);
    Shift      = stdpData(13);
    SinAmpl    = stdpData(14);
    SinPeriod  = stdpData(15);
    damper     = stdpData(16);
    % maxAbsdW       = stdpData(17);    % Not used here
    % preFreqFactor  = stdpData(18);    % Not used here
    % postFreqFactor = stdpData(19);    % Not used here
        
    deltas = deltast * tau2;
    
    factors = zeros(size(deltas));
    if Aplus ~= 0
        factors = factors + Aplus * (1 - 1 ./ deltas) .^ S;
    end
    if epsilon ~= 0 && Aminus ~= 0
        factors = factors + epsilon * Aminus * (1 - 1 ./ deltas) .^ S2;
    end
    if AC ~= 0
        factors = factors + AC * exp(TimeCosExp * abs(deltas)) * 100 .* cos(TimeCos * deltas);
    end
    if AS ~= 0 && SinAmpl ~= 0
        factors = factors - AS * exp(TimeSinExp * abs(TimeAbs * deltas + Shift)) * SinAmpl .* sin(SinPeriod * (deltas + Shift));
    end
    
    factors = factors / damper;
    
    % Replace NaNs with zeros
    factors(isnan(factors)) = 0;
    
end

% Keep it in sync with the function of the same name in worker/STDP/TabulatedStdpMechanism.h
function factors = TabulatedAtomicCorrection(stdpXYmodel, deltast)

    x = stdpXYmodel(:, 1);
    y = stdpXYmodel(:, 2);

    % Linear interpolation
    factors = interp1(x, y, deltast);

    % Zeros for out of range argument values
    factors(deltast < x(1)) = 0;
    factors(deltast > x(end)) = 0;
    
end

%%
function DoPlotEpsilonSmoothing(numPts)
    
    global epsAt0p5T
    
    s = -3 * log(epsAt0p5T);
    
    numMods = 4;
    color = {'r', 'g', 'b', 'm'};
    figure('Name', 'EpsilonSmoothing', 'NumberTitle', 'off');
    T = 1 : 4;
    x = linspace(0, 5, numPts);
    for i = 1 : length(T)
        plot(x, EpsilonSmoothing(x, T(i), s), color{i});
        hold on
    end
    for i = 1 : length(T)
        plot(T(i) / 2, epsAt0p5T, [color{i}, 'o']);
    end
    axis([x(1), x(end), -0.1, 1.1]); 
    grid on
    tit = sprintf('Factors in STDP models vs impulse age (epsAt0p5T = %g)', epsAt0p5T);
    title(tit); 
    xlabel('Current time - Impulse time, ms'); 
    ylabel('Factor'); 
 
    leg = cell(numMods, 1);
    for i = 1 : numMods
        leg{i} = sprintf('{STDP period = }{%g} ms', T(i));
    end
    
    legend(leg);

end

% Keep it in sync with the function of the same name in worker/STDP/BaseStdpMechanism.h
function value = EpsilonSmoothing(x, T, s)
    
    value = zeros(size(x));
    idx = x < T;
    value(idx) = exp(s * x(idx) .^ 2 ./ (x(idx) .^ 2 - T ^ 2));
    
end
