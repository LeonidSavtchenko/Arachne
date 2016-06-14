function [movAvg, movStdDev] = GetMovAvgStdDev(signal)
%% Given vector, compute moving average and moving sample standard deviation.
%  Native Matlab functions aren't used here because it would require Financial toolbox installed.

    global winSizeDivisor
    
    % Chose window width
    sigLen = length(signal);
    window = round(sigLen / winSizeDivisor);
    if rem(window, 2) == 0
        window = window + 1;    % must be odd
    end
    delta = (window - 1) / 2;
    
    %% Initialize moving sum corresponding to previous iteration
    movSum = 0;

    %% Preallocate array for moving average:
    %
    %                  1       i+delta
    %  movAvg(i) = --------- *   sum     signal(k)
    %              2*delta+1   k=i-delta
    %
    movAvg = nan(sigLen, 1);

    %% Initialize moving sum of "square of difference between signal and its moving average" corresponding to previous iteration
    movSumSqDiff = 0;

    %% Preallocate array for moving sample standard deviations:
    %
    %                      /    1      i+delta                                 \
    %  movStdDev(i) = sqrt | ------- *   sum     ( signal(k) - movAvg(k) ) ^ 2 |
    %                      \ 2*delta   k=i-delta                               /
    %
    movStdDev = nan(sigLen, 1);

    %% Loop through x samples
    for k = 1 : sigLen

        %% Update moving sum
        movSum = movSum + FilterNaN(signal(k));
        if k > window
            % Remove the oldest sample in the window from the sum
            iter = k - window;
            movSum = movSum - FilterNaN(signal(iter));
        end

        %% Compute moving average for iteration k - delta
        if k >= window
            movAvg(k - delta) = movSum / window;
        end

        %% Update sum of "square of difference between signal and its moving average"
        if k >= window
            movSumSqDiff = movSumSqDiff + FilterNaN((signal(k - delta) - movAvg(k - delta)) ^ 2);
        end
        if k > window
            % Remove the oldest sample in the window from the sum
            iter = k - window;
            movSumSqDiff = movSumSqDiff - FilterNaN((signal(iter) - movAvg(iter)) ^ 2);
        end

        % Make sure that finite-precision arithmetics did not make magic things
        % (otherwise complex number would appear and spoil further calculations)
        if movSumSqDiff < 0
            movSumSqDiff = 0;
        end
        
        %% Compute moving sample standard deviation for iteration k - 2*delta
        if k >= window + 2 * delta
            movStdDev(k - 2*delta) = sqrt(movSumSqDiff / (window - 1));
        end

    end
    
    %% Propagate NaNs
    movAvg(isnan(signal)) = nan;
    movStdDev(isnan(signal)) = nan;

end


function x = FilterNaN(x)
    if isnan(x)
        x = 0;
    end
end