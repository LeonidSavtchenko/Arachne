function PlotSCM(plotIndex, M, name)
%% Plot one matrix of synaptic conductance given 2 x 2 plot group

    subplot(2, 2, plotIndex)
    
    clow = min(min(M));
    chigh = max(max(M));
    if chigh == clow
        chigh = clow + 1;
    end
    c = [clow, chigh];
    
    imagesc(M, c);
    
    axis equal
    set(gca, 'ylim', [0.5, size(M, 1) + 0.5]);
    set(gca, 'xlim', [0.5, size(M, 2) + 0.5]);
    title(name);
    
    colorbar
    
end