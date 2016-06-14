function PlotSynCondVsTime(time, synCond, suffix, watchedSynIdx)
%% Plot synaptic conductance vs time given synapses

    xRange = [time(1), time(end)];
    for i = 1 : size(watchedSynIdx, 1)
        figure('Name', sprintf('Synaptic Conductance g_%s(%i, %i)', suffix, watchedSynIdx(i, 1), watchedSynIdx(i, 2)), 'NumberTitle', 'off');
        plot(time, synCond(i, :));
        grid on
        set(gca, 'xlim', xRange);
        xlabel('time, ms');
        ylabel('conductance, S');
        title('Synaptic conductance');
    end

end