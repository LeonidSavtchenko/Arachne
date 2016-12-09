function CleanUpIODirs()
%% Clean up HPC kernel I/O directories

    global continuationMode
    
    command = ScriptCallCommand('clean_up');
    
    arg = num2str(~continuationMode);
    
    command = strjoin({command, arg}, ' ');
    
    disp('Cleaning up HPC kernel I/O directories ...');
    
    status = system(command);
    if status ~= 0
        msg = ['Failed to clean up HPC kernel I/O directories.\n', ...
            'Make sure that the script "Core\\scripts\\win-win\\params.bat" specifies WORKERDIR correctly or try with remoteHPC = true.'];
        error(sprintf(msg)); %#ok<SPERR>
    end
    
end