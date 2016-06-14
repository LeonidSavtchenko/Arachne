function PlotPresynapticVoltages(time, voltage, suffix, watchedCellIdx)
%% Plot presynaptic voltage vs time for the selected cells

    if suffix == 'e'
        color = 'r';
    else
        color = 'b';
    end
    xRange = [time(1), time(end)];
    for i = 1 : size(watchedCellIdx, 1)
        figure('Name', sprintf('Voltage (%s-%i)', suffix, watchedCellIdx(i)), 'NumberTitle', 'off');
        plot(time, voltage(i, :), color);
        hold on
        plot(xRange, [0, 0], 'k');
        grid on
        set(gca, 'xlim', xRange);
        xlabel('time, ms');
        ylabel('voltage, mV');
        title('Presynaptic voltage');
        str = { sprintf('%s-cell with index %i', suffix, watchedCellIdx(i)), 'Spike threshold' };
        legend(str);
    end

end