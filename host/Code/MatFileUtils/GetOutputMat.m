function GetOutputMat(getFromSnapshot)
%% Grab output MAT-file from remote cluster or HPC kernel directory

    global remoteHPC zipMatFiles outFileName

    if ispc && ~remoteHPC
        % Copy output MAT-file from HPC kernel directory to Matlab host directory
        disp('Taking output MAT-file ...');
        command = sprintf('call Code\\scripts\\win-win\\copy_output.bat %i', getFromSnapshot);
    elseif ispc && remoteHPC
        % Download output MAT-file from the head node of HPC cluster
        if zipMatFiles
            disp('Zipping, downloading, and unzipping output MAT-file ...');
        else
            disp('Downloading output MAT-file ...');
        end
        command = sprintf('call Code\\scripts\\win-lin\\download.bat %i %i', getFromSnapshot, zipMatFiles);
    elseif isunix && ~remoteHPC
        % Copy output MAT-file from HPC kernel directory to Matlab host directory
        disp('Taking output MAT-file ...');
        command = sprintf('bash Code/scripts/lin-lin/copy_output.sh %i', getFromSnapshot);
    elseif isunix && remoteHPC
        error('Not supported mode: isunix && remoteHPC');
    else
        error('Unknown OS');
    end
    
    status = system(command);
    if status ~= 0
        error('Failed to grab the file "output.mat".');
    end
    
    %% Copy output MAT-file with specified new name
    if ~strcmp(outFileName, 'output.mat')
        copyfile('output.mat', outFileName);
    end

end