function CheckExe()
%% Check if the file "gs.exe" is present

    global remoteHPC
    
    if remoteHPC
        command = 'call Core\scripts\win-lin\check_file.bat';
    
        arg1 = 'gs.exe';
        command = strjoin({command, arg1}, ' ');
    
        disp('Checking the file gs.exe on the cluster ...');
        status = system(command);
    
        if status ~= 0
            error('Cannot launch the simulation. The file gs.exe is missing.');
        end
    end
    
end
