function PlotGTonicGABA(time, GTonicGABA)
%% Plot GTonicGABA vs time

    figure('Name', 'GTonicGABA', 'NumberTitle', 'off');
    plot(time, GTonicGABA);
    
    tRange = [time(1), time(end)];
    set(gca, 'xlim', tRange);
    grid on
    xlabel('time, ms');
    ylabel('GTonicGABA, S');
    title('Tonic current conductance');
    
end