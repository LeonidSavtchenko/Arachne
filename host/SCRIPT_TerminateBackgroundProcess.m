function SCRIPT_TerminateBackgroundProcess()
%% Request termination of HPC kernel process running in background mode

    global c4ePeriodSec

    c4ePeriodSec = 5;
    
    AddPaths();
    
    BasicParams();
    
    % Make sure that the kernel is running at the moment
    disp('Checking whether HPC kernel is running in background mode ...');
    isRunning = CheckIfRunning();
    if ~isRunning
        disp('HPC kernel process is not active -- nothing to terminate.');
        return
    end

    command = ScriptCallCommand('send_command');

    disp('Requesting HPC kernel process for termination ...');
    status = system([command, ' terminate']);
    if status ~= 0
        error('Failed to do the request.');
    end
    disp('Request submitted.');

    while true
        
        % Wait
        pause(c4ePeriodSec);

        % Check if HPC kernel process is alive
        isRunning = CheckIfRunning();
        if ~isRunning
            disp('HPC kernel has terminated.');
            disp('Now you can launch the script "START_GammaSimulator.m" to grab simulation results or continue simulation from the same point (if parameter "saveIntermMat" was specified as True).');
            break
        else
            disp('HPC kernel has not terminated yet ...');
        end
    end

end