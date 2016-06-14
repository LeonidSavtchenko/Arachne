function PlotCaColormap(time, dt, CaColormap, watchedAstroIdx)
%% Plot calcium colormap,
%  indicate watched astrocytes with horizontal lines

    figure('Name', 'Ca colormap', 'NumberTitle', 'off');

    num_a = size(CaColormap, 1);
    
    astroIdx = 1 : num_a;
        
    clow = min(min(CaColormap));
    chigh = max(max(CaColormap));
    if chigh == clow
        chigh = clow + 1;
    end
    c = [clow, chigh];
    
    imagesc(time, astroIdx, CaColormap, c);
    
    xlim = [-dt / 2, time(end) + dt / 2];
    
    % Indicate watched astrocytes with horizontal lines
    numWatchedAstro = length(watchedAstroIdx);
    green = [0.1, 0.7, 0.2];
    if numWatchedAstro > 0
        hold on
        for i = 1 : numWatchedAstro
            plot(xlim, double(watchedAstroIdx(i)) * [1, 1], 'color', green);
        end
        legend('watched astro cell(s)');
    end
    
    set(gca, 'xlim', xlim);
    set(gca, 'ylim', [0.5, num_a + 0.5]);
    
    % Make y = 1 in the bottom and y = num_a in the top
    set(gca, 'ydir', 'normal');
    
    xlabel('time, ms');
    ylabel('Astrocyte index');
    title('Calcium colormap');
    
    colorbar
    
end