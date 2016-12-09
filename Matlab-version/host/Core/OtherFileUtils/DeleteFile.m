function DeleteFile(varargin, filename)
%% Delete the file

    global remoteHPC
    
    command = ScriptCallCommand('delete_file');
    
    if xor(ispc, remoteHPC)
        % Windows separator
        separator = '\';
    else
        % Linux separator
        separator = '/';
    end
    
    filePath = strjoin(varargin, separator);
    command = [command, ' ', filePath, ' ', filename];
    isPresent = ~system(command);
    
end