function valid = valPred_modParamsTextArea(lines, varName)

    global modFileNameToUninitParamsNamesStruct_e
    global modFileNameToUninitParamsNamesStruct_i
    
    if isempty(lines)
        valid = true;
        return
    end
    
    % Initialize global variables "modParams_e" and "modParams_i",
    % make sure that there is no exception thrown
    valid = evalTextArea(lines, varName);
    if ~valid
        return
    end
    
    global modParams_e modParams_i
    
    neuronType = varName(end);
    if neuronType == 'e'
        valid = Validate(modFileNameToUninitParamsNamesStruct_e, modParams_e);
    elseif neuronType == 'i'
        valid = Validate(modFileNameToUninitParamsNamesStruct_i, modParams_i);
    else
        assert(false);
    end
    
end

function valid = Validate(modFileNameToUninitParamsNamesStruct, textAreaParamsStruct)

    valid = false;
    
    modFileNames = fields(modFileNameToUninitParamsNamesStruct);
    
    numModFilesWithUninitParams = 0;
    for i = 1 : length(modFileNames)
        modFileName = modFileNames{i};
        uninitParamsNames = modFileNameToUninitParamsNamesStruct.(modFileName);
        numUninitParams = length(uninitParamsNames);
        
        if numUninitParams == 0
            % This MOD file does not require assignment of any parameters - just skip it
            continue
        else
            numModFilesWithUninitParams = numModFilesWithUninitParams + 1;
        end
        
        if ~isfield(textAreaParamsStruct, modFileName)
            % No one parameter is assigned for this MOD file
            return
        end
        
        textAreaParamsForModFile = textAreaParamsStruct.(modFileName);
        
        for j = 1 : numUninitParams
            uninitParamName = uninitParamsNames{j};
            if ~isfield(textAreaParamsForModFile, uninitParamName)
                % Some parameter is not assigned for this MOD file
                return
            end
            
            assignedParamValue = textAreaParamsForModFile.(uninitParamName);
            
            try
            	valid_ = isnumeric(assignedParamValue) && ~isnan(assignedParamValue) && ~isinf(assignedParamValue) && isreal(assignedParamValue);
            catch
                % Bad assigned value (array)
                return
            end
            if ~valid_
                % Bad assigned value (cell array, string, NaN or infinity)
                return
            end
        end
        
        if numUninitParams < length(fields(textAreaParamsForModFile))
            % Some extra parameters are assigned for this MOD file
            return
        end
        
    end
    
    if numModFilesWithUninitParams < length(fields(textAreaParamsStruct))
        % Parameters of some extra MOD files are assigned in text area
        return
    end
        
    valid = true;
    
end
