function command = ScriptCallCommand(scriptName)
    
    global remoteHPC
    
    if ispc && ~remoteHPC
        command = ['call Core\scripts\win-win\', scriptName, '.bat'];
    elseif ispc && remoteHPC
        command = ['call Core\scripts\win-lin\', scriptName, '.bat'];
    elseif isunix && ~remoteHPC
        command = ['bash Core\scripts\lin-lin\', scriptName, '.sh'];
    elseif isunix && remoteHPC
        error('Not supported mode: isunix && remoteHPC');
    else
        error('Unknown OS');
    end

end

