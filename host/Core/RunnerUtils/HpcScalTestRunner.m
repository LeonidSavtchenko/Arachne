function HpcScalTestRunner()

    % Minimum and maximum number of processes (cluster nodes) used in scalability test.
    % Maximum number of threads (processor cores).
    global minNP maxNP maxNT

    timer = zeros(maxNT, maxNP - minNP + 1);
    for np = minNP : maxNP
        for nt = 1 : maxNT
            % Call gamma simulation kernel
            fprintf('Calling HPC kernel for np = %i, nt = %i ... ', np, nt);
            tic;
            LaunchSimulation(np, nt);
            timer(nt, np - minNP + 1) = toc;
            toc
            disp('HPC kernel finished.');
        end
    end

    if maxNP == 1 || maxNT == 1
        % Plot single scalability curve
        PlotScalabilityCurve(maxNP, maxNT, timer);
    else
        % Plot set of scalability curves
        PlotScalabilityCurveSet(minNP, maxNP, maxNT, timer);
    end
    
end


%% Plot single scalability curve
%  (the case when maxNP == 1 or maxNT == 1)    
function PlotScalabilityCurve(maxNP, maxNT, timer)

    global remoteHPC
    
    if maxNT == 1
        maxNPT = maxNP;
        xlab = 'Number of processes';
    else
        maxNPT = maxNT;
        xlab = 'Number of threads';
    end
    
    figure('Name', 'Scalability', 'NumberTitle', 'off');
    plot(1 ./ timer, '.-');
    grid on;
    set(gca, 'xtick', 0 : (maxNPT + 1));
    xlabel(xlab);
    ylabel('1 / Time of execution, sec^{-1}');
    
    % Plot perfect scalability line
    k = 100;
    x = [0, k];
    y = [0, k / timer(1)];
    ylim = get(gca, 'ylim');
    hold on;
    plot(x, y, 'g');
    set(gca, 'xlim', [0, maxNPT + 1]);
    ylim(1) = 0;
    set(gca, 'ylim', ylim);
    
    % Move scalability curve forward and perfect scalability line backward
    ch = get(gca, 'children');
    tmp = ch(1);
    ch(1) = ch(2);
    ch(2) = tmp;
    set(gca, 'children', ch);
    
    % Prepare legend
    leg = {'Perfect scalability', 'Scalability curve'};
    
    if ~remoteHPC
        % Plot vertical line for number of cores
        numCores = feature('numCores');
        plot(numCores * [1, 1], ylim, 'm--');
        
        leg = [leg, 'Num. cores'];
        
        % Plot vertical line for visible number of processors
        numProcessors = str2double(getenv('NUMBER_OF_PROCESSORS'));
        plot(numProcessors * [1, 1], ylim, 'r--');
        
        leg = [leg, 'CPU capacity'];
    end
    
    legend(leg, 'location', 'northwest');

end


%% Plot set of scalability curves
%  (the case when maxNP > 1 and maxNT > 1)
function PlotScalabilityCurveSet(minNP, maxNP, maxNT, timer)
        
    leg = cell(1, maxNT);

    colors = [1, 0, 0;  % r
              0, 1, 0;  % g
              0, 0, 1]; % b
    numColors = size(colors, 1);

    figure('Name', 'Scalability', 'NumberTitle', 'off');

    pattern = 'nt = %i';
    for numThreads = 1 : maxNT
        idx = 1 + rem(numThreads - 1, numColors);
        basicColor = colors(idx, :);
        color = basicColor * 0.9 ^ (double(numThreads) - 1);
        plot(minNP : maxNP, 1 ./ timer(numThreads, :), '.-', 'color', color);
        hold on;
        text(double(minNP), 1 / timer(numThreads, 1), sprintf(pattern, numThreads));
        leg{numThreads} = sprintf('Num. threads per process = %i', numThreads);
    end

    set(gca, 'xtick', minNP - 1 : maxNP + 1);
    set(gca, 'xlim', [minNP - 1, maxNP + 1]);
    ylim = get(gca, 'ylim');
    ylim(1) = 0;
    set(gca, 'ylim', ylim);

    grid on;

    xlabel('Number of processes');
    ylabel('1 / Time of execution, sec^{-1}');

    legend(leg, 'location', 'northwest');
    
end