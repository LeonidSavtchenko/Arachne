function TranslateModFiles()

    global importMod_e
    global modDirPath_e
    global modFileNameToUninitParamsNamesStruct_e
    global importMod_i
    global modDirPath_i
    global modFileNameToUninitParamsNamesStruct_i
    
    TranslateModFilesForOneNeuronType(importMod_e, modDirPath_e, modFileNameToUninitParamsNamesStruct_e, 'e');
    TranslateModFilesForOneNeuronType(importMod_i, modDirPath_i, modFileNameToUninitParamsNamesStruct_i, 'i');
end

function TranslateModFilesForOneNeuronType(importMod, modDirPath, modFileNameToUninitParamsNamesStruct, neuronType)

    if ~importMod
        return
    end
    
    modFilesNames = fields(modFileNameToUninitParamsNamesStruct);
    
    numModFiles = length(modFilesNames);
    paths_to_mod = cell(numModFiles, 1);
    for i = 1 : numModFiles
        paths_to_mod{i} = fullfile(modDirPath, [modFilesNames{i}, '.mod']);
    end

    % !! temporary solution
    out_path = fullfile(cd, '..', 'worker_tests', 'ModTranslatorTest', 'ModTranslatorTest');
        
    arr_pars = mod_params_parser(neuronType);
    for i = 1 : length(paths_to_mod)
        path_to_mod = paths_to_mod{i};
        [~, mod_file_name, ~] = fileparts(path_to_mod);
        fprintf('Generating %s.h ...\n', mod_file_name);
        
        user_init_params = struct;
        for j = 1 : length(arr_pars)
            if strcmp(arr_pars{j}.filename, mod_file_name)
                user_init_params = arr_pars{j};
                break
            end
        end
             
        mod_file_name = [mod_file_name, '_', neuronType]; %#ok<AGROW>
        
        parameters_blocks = ParseModFile(path_to_mod);
        GenerateCppHdrFile(mod_file_name, parameters_blocks, out_path, user_init_params);
        
        fprintf('Generating %s.cpp ...\n', mod_file_name);
        GenerateCppSrcFile(mod_file_name, parameters_blocks, out_path);
    end
    
end
