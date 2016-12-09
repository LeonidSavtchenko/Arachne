function PlotRastergram(num_e, num_i, t_final, idx_e_spikes, t_e_spikes, idx_i_spikes, t_i_spikes, watchedCellIdx_e, watchedCellIdx_i, watchedAstroIdx, dt, numSession, firstStepSession, modeSession, imageMode, imageSession)
%% Plot rastergram, e.g. indexes of spiked cells vs time

    delta = 0.1;
    
    num_spikes_e = length(idx_e_spikes);
    num_spikes_i = length(idx_i_spikes);
    
    watchedCellNum_e = length(watchedCellIdx_e);
    watchedCellNum_i = length(watchedCellIdx_i);
    watchedAstroNum = length(watchedAstroIdx);
    
    figure('Name', 'Rastergram', 'NumberTitle', 'off');
    axes;
    set(gca, 'Box', 'on');
    hold on
    
    % The array of plot handles which will have a legend entry
    h = [];
    
    % Plot rastergram points
    if num_spikes_e > 0
        h(end + 1) = plot(t_e_spikes, idx_e_spikes + num_i, '.r');
    end
    if num_spikes_i > 0
        h(end + 1) = plot(t_i_spikes, idx_i_spikes, '.b');
    end
    
    % Indicate all watched cells with horizontal lines
    grey = [0.5, 0.5, 0.5];
    green = [0.1, 0.7, 0.2];
    xRange = [0, t_final];
    for j = 1 : watchedCellNum_e
         handle = plot(xRange, num_i + watchedCellIdx_e(j) * int32([1, 1]), 'Color', grey);
         if j == 1
            h(end + 1) = handle; %#ok<AGROW>
         end
    end
    for j = 1 : watchedCellNum_i
        handle = plot(xRange, watchedCellIdx_i(j) * int32([1, 1]), 'Color', grey);
        if j == 1 && watchedCellNum_e == 0
            h(end + 1) = handle; %#ok<AGROW>
        end
    end
    for j = 1 : watchedAstroNum
        handle = plot(xRange, num_i + watchedAstroIdx(j) * int32([1, 1]), 'Color', green);
        if j == 1
            h(end + 1) = handle; %#ok<AGROW>
        end
    end
    
    % Show horizontal boundary between e-cells and i-cells
    plot(xRange, double(num_i) * [1, 1] + 0.5, '--k');
    
    if imageMode
        % Show horizontal boundary between e-cells and session titles
        plot(xRange, double(num_i + num_e) * [1, 1] + 0.5, '--k');
    
        % Show vertical boundary between sessions
        for i = 2 : numSession
            t_start_session = (double(firstStepSession(i)) - 0.5) * dt;
            plot(t_start_session * [1, 1], [0, double(num_e + num_i + 1) * (1 + delta)], 'Color', grey);
        end
        for i = 1 : numSession
            if modeSession(i) == 0
                color = [0.6, 0.1, 0.5];
                str = 'Memorization';
            else
                color = [0.1, 0.6, 0.5];
                str = 'Recall';
            end
            x = double(firstStepSession(i) + firstStepSession(i + 1)) * double(dt) / 2;
            y = double(num_e + num_i + 1) * (1 + 0.75 * delta);
            text(x, y, str, 'HorizontalAlignment', 'center', 'VerticalAlignment', 'top', 'Color', color);
            
            x = (double(firstStepSession(i)) - 0.5) * dt;
            currentImage = imageSession(:, i);
            activeCells = find(currentImage);
            imageLength = length(activeCells);
            if imageLength ~= 0
                activeCells = activeCells + double(num_i);
                x = x * ones(imageLength, 1);
                plot(x, activeCells, '.', 'Color', color);
                plot(x, activeCells, 'o', 'Color', color);
            end
        end
    end
    
    ytick_i = num_i;
    ytick_e = num_i + num_e;

    set(gca, 'XLim', xRange);
    yLim = double([0, num_e + num_i + 1]);
    if imageMode
        yLim(2) = yLim(2) * (1 + delta);
    end
    set(gca, 'YLim', yLim);
    set(gca, 'YTick', [ytick_i, ytick_e]);
    
    xlabel('time, ms');
    ylabel('Cell index');
    
    % Prepare and show legend
    leg = {};
    if num_spikes_e ~= 0
        leg{end + 1} = 'e-cells';
    end
    if num_spikes_i ~= 0
        leg{end + 1} = 'i-cells';
    end
    if watchedCellNum_e ~= 0 || watchedCellNum_i ~= 0
        leg{end + 1} = 'watched pyra cell(s)';
    end
    if watchedAstroNum ~= 0
        leg{end + 1} = 'watched astro cell(s)';
    end
    if ~isempty(leg)
        if ~imageMode
            loc = 'NorthEast';
        else
            loc = 'SouthEast';
        end
        legend(h, leg, 'Location', loc);
    end
    
    title('Rastergram');
    
end
