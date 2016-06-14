function RunAndMonitor()

    global remoteHPC scalTest continuationMode imageMode recallMode backgroundMode
    global guiType GuiTypes
    global image %#ok<NUSED>
    
    
    %% Image external drive input
    if imageMode
        CreateImageDrives();
    end
    
    if guiType == GuiTypes.ContinueOldSession;
        continuationMode = true;
    end
    
    if ~continuationMode
        recallMode = false;
    elseif imageMode
        choice = '';
        while isempty(choice)
            choice = questdlg('Should gamma simulator memorize or recall this image?', 'STDP ON/OFF option', 'Memorize', 'Recall', 'Memorize');
        end
        recallMode = strcmp(choice, 'Recall');
    else
        recallMode = true;
    end
            
    %% Check the file gs.exe
    CheckExe();
    
    %% Check the number of active nodes
    CheckNodes();
    
    %% Clean up HPC kernel I/O directories
    CleanUpIODirs();
    
    % Clean up old label 'complete'
    isComplete = CheckFileExists('iofiles', 'complete');
    if isComplete
        DeleteFile({'iofiles'}, 'complete');
    end
    
    if ~continuationMode
        %% Prepare input data file "input.mat,"
        %  check memory requirements
        PrepareInputFromGUI();
    
        %% Delete old output data file "output.mat", if any
        if ispc
            system('if exist output.mat del output.mat');
        elseif isunix
            system('rm output.mat -f');
        else
            error('Unknown OS');
        end
    
        %% Deploy input MAT-file to cluster or HPC kernel directory
        DeployMatFile('input');
    end
    
    if imageMode
        %% Save the image to the file "image.mat"
        disp('Creating image MAT-file ...');
        save('image.mat', 'image');
    
        %% Deploy image MAT-file to cluster or HPC kernel directory
        DeployMatFile('image');
    end
    
    %% Run HPC kernel / perform scalability test
    if ~scalTest
        % Run HPC kernel in foreground or background mode
        HpcRunner();
    else
        % Perform scalability test
        % (HPC kernel is run in foreground mode only)
        HpcScalTestRunner();
        return
    end
    
    %% If simulation is running in background on this machine, do monitoring of the process
    if backgroundMode && ~remoteHPC
        MonitorBackgroundProcess();
    end
    
    %% Check if simulation finished successfully
    isComplete = CheckFileExists('iofiles', 'complete');
    msg = sprintf(['    The host detected that some error occurred in HPC kernel.\n', ...
                   '    If the above text contains a message starting with ">>>>>", then consider this message. ', ...
                   '(The error, most likely, caused by bad input data.)\n', ...
                   '    If the text does not contain the message, then this error, most likely, indicates a bug in the program. ', ...
                   'Please contact Sergey in that case.']);
    assert(isComplete, msg);
    DeleteFile({'iofiles'}, 'complete');

end