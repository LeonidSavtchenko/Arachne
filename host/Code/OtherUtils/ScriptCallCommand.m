function command = ScriptCallCommand(scriptName)
    
    global remoteHPC
    
    if ispc && ~remoteHPC
        command = ['call Code\scripts\win-win\', scriptName, '.bat'];
    elseif ispc && remoteHPC
        command = ['call Code\scripts\win-lin\', scriptName, '.bat'];
    elseif isunix && ~remoteHPC
        command = ['bash Code\scripts\lin-lin\', scriptName, '.sh'];
    elseif isunix && remoteHPC
        error('Not supported mode: isunix && remoteHPC');
    else
        error('Unknown OS');
    end

end

