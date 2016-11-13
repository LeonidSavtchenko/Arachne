function MonitorBackgroundProcess()
%% Do monitoring of the background simulation process

    global c4sPeriodSec
    
    fprintf('Start monitoring with a delay of %s sec. between requests ...\n', num2str(c4sPeriodSec));
    
    % Wait
    pause(c4sPeriodSec);
    
    while true
        % Read and print current progress
        % (the progress file can be not found if simulation started recently
        % or HPC kernel refreshes it too frequently)
        progress = GetCurrentProgress();
        if ~isempty(progress)
            disp(progress);
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
            break
        end
    end
    
end