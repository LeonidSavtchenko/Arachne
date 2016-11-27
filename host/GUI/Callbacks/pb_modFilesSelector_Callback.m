function pb_modFilesSelector_Callback(hObject, ~)
%% Callback function for MOD files selector pushbuttons

    global params
    global modDirPath_e
    global modFileNamesWithUninitParams_e
    global modFileNames_e
    global modFileNameToUninitParamsNamesStruct_e
    global modDirPath_i
    global modFileNamesWithUninitParams_i
    global modFileNames_i
    global modFileNameToUninitParamsNamesStruct_i
    global FileSelectorIds
    
    % Determine the neuron type (e or i)
    userData = get(hObject, 'UserData');
    panIdx = userData(1);
    parIdx = userData(2);
    pbName = params{panIdx}{parIdx}.name;
    neuronType = pbName(end);
    
    [fileNames, dirPath] = UIGetFile(...
        FileSelectorIds.ModFiles, ...
        {'*.mod', 'MOD files (*.mod)'; '*.*', 'All files (*.*)'}, ...
        'Select MOD file(s)', ...
        'MultiSelect', 'on');
    if isequal(fileNames, 0) || isequal(fileNames, 0)
        % Cancelled by user
        modFileNames = {};
        modFileNamesWithUninitParams = {};
        modFileNameToUninitParamsNamesStruct = struct;
        textAreaPrompt = {};
    else
        modFilePaths = fullfile(dirPath, fileNames);
        [modFileNames, modFileNamesWithUninitParams, modFileNameToUninitParamsNamesStruct, textAreaPrompt] = LoadModFiles(modFilePaths, neuronType);
    end
    
    userData = get(hObject, 'UserData');
    panIdx = userData(1);
    parIdx = userData(2);
    
    % Text area is the next element after the MOD file selector pushbutton
    textAreaName = params{panIdx}{parIdx + 1}.name;

    % !! temp solution
    neuronType = textAreaName(end);
    if neuronType == 'e'
        
        modDirPath_e = dirPath;
        modFileNames_e = modFileNames;
        modFileNamesWithUninitParams_e = cell(length(modFileNamesWithUninitParams), 1); %!!not needed?
        for i = 1 : length(modFileNamesWithUninitParams)          
            modFileNamesWithUninitParams_e{i, 1} = modFileNamesWithUninitParams{i}; 
        end

        modFileNameToUninitParamsNamesStruct_e = modFileNameToUninitParamsNamesStruct;

    elseif neuronType == 'i'
        
        modDirPath_i = dirPath;
        modFileNames_i = modFileNames;
        modFileNamesWithUninitParams_i = cell(length(modFileNamesWithUninitParams), 1);
        for i = 1 : length(modFileNamesWithUninitParams)          
            modFileNamesWithUninitParams_i{i, 1} = modFileNamesWithUninitParams{i}; 
        end

        modFileNameToUninitParamsNamesStruct_i = modFileNameToUninitParamsNamesStruct;
        
    end

    % Put textAreaPrompt into the text area
    taHandler = params{panIdx}{parIdx + 1}.handlers(2);
    set(taHandler, 'String', textAreaPrompt);
    
    % Update corresponding global variable
    params{panIdx}{parIdx + 1}.value = textAreaPrompt;

    % Call generic callback function for the text area
    generic_Callback(taHandler);
    
end