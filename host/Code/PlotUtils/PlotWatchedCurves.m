function PlotWatchedCurves(time, watchedIdx, watchedY, plotStyle, figNamePat, title, parName)
%% Plot the watched curves vs time for the selected pyramidal cells, astrocytes or synapses
    
    global plotStyles
    
    legendPat = [parName, ' with index %i'];
    
    switch plotStyle
        case plotStyles.DoNotPlot
            return
        case plotStyles.PlotCurvesSeparately
            PlotWatchedCurvesSeparately(time, watchedIdx, watchedY, figNamePat, title, legendPat)
        case plotStyles.PlotCurvesTogether
            PlotWatchedCurvesTogether(time, watchedIdx, watchedY, figNamePat, title, legendPat)
        otherwise
            assert(false, 'Bad plotStyle');
    end
        
end

function PlotWatchedCurvesSeparately(time, indexes, values, figNamePat, title, legendPat)
%% Plot each curve separately (on individual figure)

    xRange = [time(1), time(end)];
    
    for i = 1 : length(indexes)
        index = indexes(i);
        figure('Name', sprintf(figNamePat, num2str(index)), 'NumberTitle', 'off');
        plot(time, values(i, :), 'm');
        legend = sprintf(legendPat, index);
        StyleActivePlot(xRange, title, legend);
    end
    
end

function PlotWatchedCurvesTogether(time, indexes, values, figNamePat, title, legendPat)
%% Plot all curves together (on the same figure)
    
    xRange = [time(1), time(end)];
    
    figure('Name', GetFigName(figNamePat, indexes), 'NumberTitle', 'off');
    
    n = length(indexes);
    
    legItems = cell(n, 1);
    
    for i = 1 : n
        plot(time, values(i, :), 'color', CurveIndexToColor(i));
        hold on
        legItems{i} = sprintf(legendPat, indexes(i));
    end
    
    StyleActivePlot(xRange, title, legItems);
    
end

function StyleActivePlot(xRange, title_, legItems)
%% Apply style given active plot

    grid on
    set(gca, 'xlim', xRange);
    xlabel('time, ms');
    title(title_);
    legend(legItems);
    
end

function name = GetFigName(figNamePat, indexes)

    str = num2str(indexes(1));
    
    for i = 2 : length(indexes)
        str = [str, ',', num2str(indexes(i))]; %#ok<AGROW>
    end
    
    name = sprintf(figNamePat, str);
    
end

function color = CurveIndexToColor(curveIdx)

    v0 = 0;
    v1 = 1;
    v2 = 1 / sqrt(2);
    v3 = 1 / sqrt(3);
    palette = [...
       v1, v0, v0; ...
       v0, v0, v1; ...
       v0, v1, v0; ...
       v2, v2, v0; ...
       v2, v0, v2; ...
       v0, v2, v2; ...
       v3, v3, v3];
    palSize = size(palette, 1);
    colorIdx = 1 + rem(curveIdx - 1, palSize);
    damper = 1 + floor((curveIdx - 1) / palSize);
    color = palette(colorIdx, :) / damper;
    
end