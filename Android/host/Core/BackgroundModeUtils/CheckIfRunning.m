function isRunning = CheckIfRunning(silentMode)
%% Checks if HPC kernel is running in background mode

    global remoteHPC availableNodes
    
    if nargin == 0
        silentMode = false;
    end
    
    command = ScriptCallCommand('check_running');
    
    if remoteHPC
        args = strjoin(availableNodes, ' ');
        command = sprintf('%s %s', command, args);
    end
    
    if silentMode
        command = [command, ' > NUL'];
    end
    
    isRunning = ~system(command);

    if ispc && ~remoteHPC
        isRunning = ~isRunning;
    end
    
end
