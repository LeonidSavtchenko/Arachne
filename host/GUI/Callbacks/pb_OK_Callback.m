function pb_OK_Callback(~, ~)

    global invalidParams hf saveInput2Output
    global guiType debugMode
    global GuiTypes
    
    if ~isempty(invalidParams)
        msg = ['The following parameters are not valid:', invalidParams];
        warndlg(msg, 'Invalid parameters');
        return
    end
    
    % Evaluate all remainders given GUI type
    if ~debugMode
        % Show error message only
        try
            EvaluateAllRemainders(guiType);
        catch ex
            warndlg(ex.message, 'Invalid parameters');
        end
    else
        % Show error message and call stack
        EvaluateAllRemainders(guiType);
    end
    
    if guiType == GuiTypes.StartFromScratch
        % Translate all the MOD files to C++ source and header files for both "e" and "i" neurons,
        % deploy the files to the worker source directory,
        % then build the worker
        TranslateModFilesDeployCppFilesAndBuildWorker();
    end
    
    % Save input parameters
    if saveInput2Output
        SaveParams('guiParams.mat');
    end
        
    close(hf);
    
    [okHandler, stopAfter] = GuiTypeToOkHandler(guiType);
    
    okHandler();
    
    if stopAfter
        return
    end
      
    % Grab output MAT-file from remote cluster or HPC kernel directory
    getFromSnapshot = false;
    GrabReadAndVisualizeResults(getFromSnapshot);

end
