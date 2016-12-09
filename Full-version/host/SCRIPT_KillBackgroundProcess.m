function SCRIPT_KillBackgroundProcess()
%% Kill HPC kernel process running in background mode

    global remoteHPC availableNodes
    
    AddPaths();
  
    BasicParams();
    
    % Make sure that the kernel is running at the moment
    disp('Checking whether HPC kernel is running in background mode ...');
    isRunning = CheckIfRunning();
    if ~isRunning
        disp('HPC kernel process is not active -- nothing to kill.');
        return;
    end

    command = ScriptCallCommand('kill');
    
    if remoteHPC
        args = strjoin(availableNodes, ' ');
        command = sprintf('%s %s', command, args);
    end

    disp('Killing HPC kernel process ...');
    status = system(command);
    if status ~= 0
        error('Failed to kill HPC kernel process.');
    end

    isRunning = CheckIfRunning();
    if ~isRunning
        disp('HPC kernel process is dead.');
    else
        error('Failed to kill HPC kernel process.');
    end

end