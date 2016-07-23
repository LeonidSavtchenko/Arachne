function CheckNodes()
%% Check the number of active nodes

    global remoteHPC
    global np
    global hostParams
    
    if ispc && remoteHPC
        command = 'call Code\scripts\win-lin\test_launching.bat';
    
        arg1 = num2str(np);
        command = strjoin({command, arg1, hostParams}, ' ');
    
        disp('Checking the number of active nodes ...');
        status = system(command);
    
        if status ~= 0
            error('Cannot launch the simulation. The number of active nodes is less than the requested number of processes.');
        end
    end
    
end

