function [modFileNames, modFileNamesWithUninitParams, modFileNameToUninitParamsNamesStruct, textAreaPrompt] = LoadModFiles(modFilePaths, neuronType)
%% Load and parse the MOD file(s).
%  Generate a prompt for the text area with parameters of the currents.
%  "modFilePaths" can be either a string or a cell array of strings.
%  "neuronType" can be either 'e' or 'i' (it is used when "textAreaPrompt" is prepared).

    if ischar(modFilePaths)
        modFilePaths = {modFilePaths};
    end
    
    modFileNames = {};
    modFileNamesWithUninitParams = {};
    modFileNameToUninitParamsNamesStruct = struct;
    
    textAreaPrompt = {};
    firstUninitParam = true;
    for i = 1 : length(modFilePaths)
        
        filePath = modFilePaths{i};
        [~, fileName, ~] = fileparts(filePath);
        
        % Update modFileNameToUninitParamsNamesStruct
        modFileNameToUninitParamsNamesStruct.(fileName) = {};
                    
        % Parse the file
        [blocks, ~] = ParseModFile(modFilePaths{i});

        % Parse "PARAMETER" block
        params = ParseParameterBlock(blocks.PARAMETER);
        
        % Check whether any parameters are uninitialized in MOD file.
        % Generate the text area prompt. 
        fileHasUninitParams = false;
        firstUninitParamForModFile = true;
        for j = 1 : length(params)
            
            paramName = strtrim(params{j});
            if strcmp(paramName, 'v')
                continue
            elseif isempty(strfind(paramName, '='))
                fileHasUninitParams = true;
                
                if firstUninitParamForModFile
                    if ~firstUninitParam
                        textAreaPrompt{end + 1} = ''; %#ok<AGROW>
                    end
                    firstUninitParam = false;
                    % Update textAreaPrompt
                    textAreaPrompt{end + 1, 1} = sprintf('%% %s.mod parameters:', fileName); %#ok<AGROW>
                end
                firstUninitParamForModFile = false;
                % Update textAreaPrompt
                textAreaPrompt{end + 1, 1} = sprintf('modParams_%s.%s.%s = nan;', neuronType, fileName, paramName); %#ok<AGROW>
                % Update modFileNameToUninitParamsNamesStruct
                uninitParams = modFileNameToUninitParamsNamesStruct.(fileName);
                uninitParams{end + 1, 1} = paramName; %#ok<AGROW>
                modFileNameToUninitParamsNamesStruct.(fileName) = uninitParams;
            end
        end
        
        modFileNames{end + 1, 1} = fileName; %#ok<AGROW>
        
        if fileHasUninitParams
            modFileNamesWithUninitParams{end + 1, 1} = fileName; %#ok<AGROW>
        end
        
    end
end
