function GrabReadAndVisualizeResults(getFromSnapshot)

    global saveInput2Output outFileName scalTest
    
    % Grab output MAT-file from remote cluster or HPC kernel directory
    GetOutputMat(getFromSnapshot);
    
    % If necessary, save input parameters to the output MAT-file
    load('output.mat', 'saveInput2Output', 'outFileName');
    if saveInput2Output
        guiParams = load('guiParams.mat'); %#ok<NASGU>
        save(outFileName, 'guiParams', '-append');
    end
    
    if ~scalTest
        % Read output MAT-file from disk and visualize results of simulation
        ReadAndVisualizeResults();
    end
    
end