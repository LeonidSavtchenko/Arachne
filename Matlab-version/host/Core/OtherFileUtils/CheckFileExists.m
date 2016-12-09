function isPresent = CheckFileExists(varargin)
%% Determine whether the file is present in HPC kernel output directory

    global remoteHPC
    
    command = ScriptCallCommand('check_file');
    
    if xor(ispc, remoteHPC)
        % Windows separator
        separator = '\';
    else
        % Linux separator
        separator = '/';
    end
    
    filePath = strjoin(varargin, separator);
    command = [command, ' ', filePath];
    isPresent = ~system(command);
    
end