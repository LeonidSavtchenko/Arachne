function success = TranslateModFilesDeployCppFilesAndBuildWorker()
%% Translate all the MOD files to C++ source and header files for both "e" and "i" neurons,
%  deploy the files to the worker source directory,
%  then build the worker.
    
    global importMod_e modDirPath_e modFileNameToUninitParamsNamesStruct_e
    global importMod_i modDirPath_i modFileNameToUninitParamsNamesStruct_i

    % Create a new or clean up the existing temporary directory
    outDirPath = fullfile(cd, '_ModTranslatorTemp'); 
    PrepareEmptyDir(outDirPath);
    
    disp('Translating MOD files into C++ code ...');
    
    % Generate C++ files (H and CPP files pairs) for all the selected MOD files
    modFileNames_e = TranslateModFilesToCpp(importMod_e, modDirPath_e, modFileNameToUninitParamsNamesStruct_e, outDirPath, 'e');
    modFileNames_i = TranslateModFilesToCpp(importMod_i, modDirPath_i, modFileNameToUninitParamsNamesStruct_i, outDirPath, 'i');

    disp('Preparing intermediary C++ code ...');
    
    % Generate cumulative H and CPP files
    GenerateCumulativeCppHdrFile(modFileNames_e, modFileNames_i, outDirPath);
    GenerateCumulativeCppSrcFile(modFileNames_e, modFileNames_i, outDirPath); 

    % Generate Inc files 
    GenerateIncFiles(modFileNames_e, modFileNames_i, outDirPath);

    % Deploy all the generated C++ files,
    % then build the worker
    success = DeployCppFilesAndBuildWorker(outDirPath);

    % Remove the temporary directory
    rmdir(outDirPath, 's');
    
end

function modFileNames = TranslateModFilesToCpp(importMod, modDirPath, modFileNameToUninitParamsNamesStruct, outDirPath, neuronType)
%% Translate all the MOD files to C++ source and header files for one neuron type ("e" or "i")

    if isempty(importMod) || ~importMod
        modFileNames = {};
        return
    end
    
    modFilesNames = fields(modFileNameToUninitParamsNamesStruct);
    
    numModFiles = length(modFilesNames);
    paths_to_mod = cell(numModFiles, 1);
    for i = 1 : numModFiles
        paths_to_mod{i} = fullfile(modDirPath, [modFilesNames{i}, '.mod']);
    end

    modFileNames = cell(length(paths_to_mod), 1);  
    arr_pars = mod_params_parser(neuronType);
    for i = 1 : length(paths_to_mod)
        path_to_mod = paths_to_mod{i};
        [~, modFileName, ~] = fileparts(path_to_mod);
        
        userInitParams = struct;
        for j = 1 : length(arr_pars)
            if strcmp(arr_pars{j}.filename, modFileName)
                userInitParams = arr_pars{j};
                break
            end
        end
             
        modFileName = [modFileName, '_', neuronType]; %#ok<AGROW>
        modFileNames{i, 1} = modFileName;
        
        fprintf('    Generating %s.h ...\n', modFileName);
        [parameters_blocks, freeLocalVars] = ParseModFile(path_to_mod);
        GenerateCppHdrFile(modFileName, parameters_blocks, freeLocalVars, outDirPath, userInitParams);
    
        fprintf('    Generating %s.cpp ...\n', modFileName);
        GenerateCppSrcFile(modFileName, parameters_blocks, outDirPath);

    end
    
end
















