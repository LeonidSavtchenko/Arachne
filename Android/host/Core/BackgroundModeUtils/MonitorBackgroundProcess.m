function completed = MonitorBackgroundProcess()
%% Do monitoring of the background simulation process.
%  This scripts normally waits until the simulation completes,
%  but it returns after the first report in the mobile mode.

    global c4sPeriodSec mobileMode
    
    fprintf('Start monitoring with a delay of %i sec. between requests ...\n', c4sPeriodSec);
    
    % Wait
    pause(c4sPeriodSec);
    
    reported = false;
    
    while true
        % Read and print current progress
        % (the progress file can be not found if simulation started recently
        % or HPC kernel refreshes it too frequently)
        progress = GetCurrentProgress();
        if ~isempty(progress)
            disp(progress);
            reported = true;
        end
        
        % Wait
        pause(c4sPeriodSec);
        
        % Check if HPC kernel process is alive
        isRunning = CheckIfRunning(true);
        if ~isRunning
            disp('Host program detected that HPC kernel process is dead.');
            disp('The last progress reported by HPC kernel:');
            progress = GetCurrentProgress();
            disp(progress);
            completed = true;
            break
        end
        
        if mobileMode && reported
            % Matlab mobile shows all the messages only after the script exits,
            % so we report progress only once and exit
            completed = false;
            break
        end
    end
        
end