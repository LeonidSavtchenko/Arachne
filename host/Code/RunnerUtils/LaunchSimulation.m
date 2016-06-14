function LaunchSimulation(np, nt)
%% Launch gamma simulator with specified parameters

    global setIter continuationMode m_steps remoteHPC hostParams backgroundMode imageMode recallMode fakeMPI
    
    % Prepare OS-specific command to run HPC kernel.
    % The command specifies the following parameters:
    % 1) the number of processes,
    % 2) the number of threads per process,
    % 3) the flag (0/1) indicating whether previous simulation session should be continued,
    % 4) the number of iterations or the time interval (in ms) to compute in this simulation session,
    % 5) the flag (0/1) indicating whether the time interval (0) or the number of iterations (1) is specified,
    % 6) list of comma-separated cluster node names to run simulation on (used only for cluster run),
    % 7) the flag (0/1) indicating whether HPC kernel should be run in background mode,
    % 8) (optional) the flag (0/1) indicating whether to disable STDP for this simulation session (presence of the argument indicates that image mode is on).
    
    if setIter
        fmt = '%i %i %i %i %i';
    else
        fmt = '%i %i %i %g %i';
    end
    args1to5 = sprintf(fmt, np, nt, continuationMode, m_steps, setIter);
    
    if ispc && ~remoteHPC
        % Run HPC kernel on this machine under Windows
        command = sprintf('call Code\\scripts\\win-win\\run.bat %s', args1to5);
        if backgroundMode
            command = ['start ', command];
        end
    elseif ispc && remoteHPC
        % Run HPC kernel on remote cluster under Linux from this machine under Windows
        command = sprintf('call Code\\scripts\\win-lin\\run.bat %s "%s"', args1to5, hostParams);
    elseif isunix && ~remoteHPC
        % Run HPC kernel on this cluster under Linux
        command = sprintf('bash Code/scripts/lin-lin/run.sh %s "%s" %i', args1to5, hostParams, backgroundMode);
    elseif isunix && remoteHPC
        % Run HPC kernel on remote machine/cluster under Windows from this machine under Linux
        error('Not supported mode: isunix && remoteHPC');
    else
        error('Unknown OS');
    end
    
    if ~remoteHPC
        % Add one more argument
        command = sprintf('%s %i', command, fakeMPI);
    end
    
    if imageMode
        % Add one more argument
        command = sprintf('%s %i', command, recallMode);
    end
   
    % Execute the command
    system(command);
    
end