function SCRIPT_CleanUp()
%% Clean up HPC kernel I/O directories

    global continuationMode
        
    AddPaths();
    
    BasicParams();
    
    % Make sure that the kernel is not running at the moment
    disp('Checking whether HPC kernel is running in background mode ...');
    isRunning = CheckIfRunning();
    if isRunning
        msg = [...
            'Cannot clean up HPC kernel I/O directories because simulation is running at the moment.\n', ...
            'Please wait until it completes or use script SCRIPT_TerminateBackgroundProcess.m or SCRIPT_KillBackgroundProcess.m to stop the simulation and then try to do cleanup once again.\n'];
        error(sprintf(msg)); %#ok<SPERR>
    end

    % Do cleanup
    continuationMode = false;
    CleanUpIODirs();
    
    disp('Done.');

end