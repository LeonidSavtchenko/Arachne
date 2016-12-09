function HpcRunner()

    global np nt remoteHPC backgroundMode
    
    disp('Launching HPC kernel ...');
    
    if ~backgroundMode
        tic;
    end
    
    % Launch gamma simulation kernel via BAT- or SH-script
    LaunchSimulation(np, nt);
    
    if ~backgroundMode
        toc
        disp('HPC kernel finished.');
    elseif ~remoteHPC
        disp('HPC kernel launched.');
    end
    
end
