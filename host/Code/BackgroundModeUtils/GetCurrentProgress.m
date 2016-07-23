function progress = GetCurrentProgress()
%% Get HPC kernel status file name that indicates the last iteration number, its duration, and total number of iterations

    global remoteHPC
    
    command = ScriptCallCommand('get_iteration');
    
    [status, cmdout] = system(command);
    
    % If host program attempts to get progress of HPC kernel from "iter *"
    % file, that has not been created yet, "dir" command returns error
    % status not equal 0. For gamma simulator missing "iter *" file is not
    % an error, it just means that HPC kernel has not reported any progress
    % yet or it refreshes the file too frequently.
    if ispc && ~remoteHPC
        if strcmp('File Not Found', cmdout(1 : end - 1))
            status = 0;
            cmdout = 'No progress';
        end
    else
        if strcmp('dir: cannot access "iter *": No such file or directory', cmdout(1 : end - 1))
            status = 0;
            cmdout = 'No progress';
        end
    end
    if status ~= 0
        % This is some real error
        error('Failed to get content of the directory "iofiles\kernel-host".');
    end
    
    i = strfind(cmdout, 'iter ');
    if isempty(i)
        % The progress file is absent.
        % It's probably because the simulation started recently.
        disp('No progress reported by HPC kernel yet or the progress file is refreshed too frequently ...');
        progress = '';
        return;
    end
    if length(i) > 2
        error('Too many files with name pattern "iter *" in the directory "iofiles\kernel-host".');
    end
    if length(i) == 2
        % It's the rare case when the new file was created, but the old one was not deleted yet
        cmdout(i(2) : end - 1) = [];
    end
    
    if remoteHPC || isunix
        % Remove double quotes that decorate file name produced by "dir" command under Linux
        cmdout = cmdout(2 : end - 1);
    end
    
    % Delete new line character
    cmdout(end) = [];
    
    % Replace "-" with "/"
    i = strfind(cmdout, '-');
    if length(i) ~= 1
        error('Bad name of the status file in the directory "iofiles\kernel-host".');
    end
    cmdout(i) = '/';
    
    % Replace ";" with ":"
    i = strfind(cmdout, ';');
    if length(i) ~= 1
        error('Bad name of the status file in the directory "iofiles\kernel-host".');
    end
    cmdout(i) = ':';
    
    progress = cmdout;
    
end