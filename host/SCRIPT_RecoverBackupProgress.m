function SCRIPT_RecoverBackupProgress()
%% Recover simulation progress from the latest backup file(s).
%  The files "output.mat" and "intermediate.mat" are subjects of recovering.

    AddPaths();
    
    BasicParams();
    
    % Make sure that the kernel is not running at the moment
    disp('Checking whether HPC kernel is running in background mode ...');
    isRunning = CheckIfRunning();
    if isRunning
        error('Cannot restore backup progress files until gamma simulator is running.');
    end

    % Select backup storage (1 or 2) to recover from
    disp('Selecting backup storage (1 or 2) to recover from ...');
    idx = SelectBackupStorage();
    fprintf('Backup storage %i will be used for recovery.\n', idx);

    % Recover files "output.mat" and "intermediate.mat" (if present) from the backup storage
    status = RecoverMatFiles(idx);
    
    disp('Recovery complete.');
    if ~status
        disp('Now you can launch the script "START_GammaSimulator.m" to grab simulation results or continue simulation from the backup point.');
    else
        disp('Now you can launch the script "START_GammaSimulator.m" to grab simulation results.');
    end

end