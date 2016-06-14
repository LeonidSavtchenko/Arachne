function PlotQualityOfRecall(idx_e_spikes, t_e_spikes, first_spike_recall, m_steps, m_steps_recall, dt, memImage, recall_frequency_e)
%% Plot the quality of image recall coefficient vs time.
%  The memorized image is a constant vector given explicitly.
%  The recalled image is a time-dependant vector derived from the rastergram.
    
    global plotQR plotQRPeriod
    
    num_e = length(memImage);
    
    % Prepare the latest recall session time grid
    recSesTimeGrid = double(m_steps - m_steps_recall + 1 : m_steps) * dt;
    
    %% Plot the inverse frequency curve
    if plotQRPeriod
        figure('Name', '1 / recall_Frequency_e', 'NumberTitle', 'off');
        plot(recSesTimeGrid, 1 ./ recall_frequency_e);
        set(gca, 'XLim', [recSesTimeGrid(1), recSesTimeGrid(end)]);
        grid on
        xlabel('time, ms');
        title('recall\_Frequency\_e^{-1}, ms');
    end
    
    if ~plotQR
        return
    end
    
    %%
    
    % Preallocate array for values of the Quality of Recall coefficient vs time
    qr = zeros(1, m_steps_recall);
        
    % Get indexes of black cells in the memorized image,
    % get the image size, i.e. the number of black cells
    memImageIdx = find(memImage);
    memImageSize = length(memImageIdx);
    
    if memImageSize == 0
        warning('The memorized image contains only white cells. The QR coefficient will be equal zero independently on the recalled image.');
    else
        %% Populate the qr array
        
        % Throw off all the spikes before the latest recall session
        idx_e_spikes = idx_e_spikes(first_spike_recall : end);
        t_e_spikes = t_e_spikes(first_spike_recall : end);
    
        % Convert flat arrays idx_e_spikes and t_e_spikes into a single jugged 2-D array containing impulse times
        spikeTimes2D = cell(num_e, 1);
        for nrnIdx = 1 : num_e
            spikeTimes2D{nrnIdx} = t_e_spikes(idx_e_spikes == nrnIdx);
        end

        % Preallocate array for the indexes of black cells in the recalled image
        % (the number of black cells will be kept in recImageSize)
        recImageIdx = zeros(1, num_e);
        
        % Prepare array with index of the next (possibly unexistent) spike for each neuron
        nextSpikeIdx = ones(1, num_e);
        for nrnIdx = 1 : num_e
            if ~isempty(spikeTimes2D{nrnIdx}) && spikeTimes2D{nrnIdx}(1) == recSesTimeGrid(1)
                nextSpikeIdx(nrnIdx) = 2;
            end
        end
        
        fprintf('Computing quality of recall ...\n');
    
        %% The main cycle.
        %  Loop by all iterations of the latest recall session.
        for stpIdx = 1 : m_steps_recall

            % Size of the recalled image, i.e. the number of black cells
            recImageSize = 0;

            % Boundaries of the time interval to search spikes within
            maxTime = recSesTimeGrid(stpIdx);
            minTime = maxTime - 1 / recall_frequency_e(stpIdx);
            
            % Loop by all e-neurons
            for nrnIdx = 1 : num_e

                % Create a shallow copy for convenience
                spikeTimes = spikeTimes2D{nrnIdx};

                % Go to the first (possibly unexistent) spike after the interval
                nsIdx = nextSpikeIdx(nrnIdx);
                if nsIdx ~= length(spikeTimes) + 1 && spikeTimes(nsIdx) <= maxTime
                    nsIdx = nsIdx + 1;
                end
                nextSpikeIdx(nrnIdx) = nsIdx;
                
                if nsIdx ~= 1 && spikeTimes(nsIdx - 1) >= minTime
                    % There is an impulse within the interval, i.e. we have a new black cell in the recalled image
                    recImageSize = recImageSize + 1;
                    recImageIdx(recImageSize) = nrnIdx;
                end
            end

            % Count the number of cells where both the memorized image and the recalled image are black
            % (we take into scope that both memImageIdx and recImageIdx are sorted)
            commonSize = 0;
            memIdx = 1;
            recIdx = 1;
            while memIdx <= memImageSize && recIdx <= recImageSize
                if memImageIdx(memIdx) == recImageIdx(recIdx)
                    commonSize = commonSize + 1;
                    memIdx = memIdx + 1;
                    recIdx = recIdx + 1;
                elseif memImageIdx(memIdx) < recImageIdx(recIdx)
                    memIdx = memIdx + 1;
                else
                    recIdx = recIdx + 1;
                end
            end

            % Compute the Quality of Recall coefficient
            qr(stpIdx) = commonSize / sqrt(recImageSize * memImageSize);
            if isnan(qr(stpIdx))
                qr(stpIdx) = 0;
            end

            % Report current progress
            if rem(stpIdx, 1000) == 0 || stpIdx == m_steps_recall
                fprintf('iter %i / %i\n', stpIdx, m_steps_recall);
            end
        end
    end
    
    %% Plot the QR curve
    figure('Name', 'Quality of Recall', 'NumberTitle', 'off');
    plot(recSesTimeGrid, qr);
    set(gca, 'XLim', [recSesTimeGrid(1), recSesTimeGrid(end)]);
    set(gca, 'YLim', [-0.1, 1.1]);
    grid on
    xlabel('time, ms');
    title('Quality of Recall');
    
end