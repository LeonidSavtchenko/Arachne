function arr_pars = mod_params_parser(neuronType)
     
    eval(['global modFileNames_', neuronType]);
    eval(['global modParams_', neuronType]);
    eval(['global modFileNamesWithUninitParams_', neuronType]);
    
    if neuronType == 'e'
        modFileNames = modFileNames_e;
        modFileNamesWithUninitParams = modFileNamesWithUninitParams_e;
    elseif neuronType == 'i'
        modFileNames = modFileNames_i;
        modFileNamesWithUninitParams = modFileNamesWithUninitParams_i;
    else
        assert(false);
    end
    
    arr_pars = cell(length(modFileNames), 1);
    
    arr_idx = 0;
    for i = 1 : length(modFileNames)     
        found_copy = false;
        for j = 1 : length(modFileNamesWithUninitParams)        
            if strcmp(modFileNames{i}, modFileNamesWithUninitParams{j})
                found_copy = true;
                break
            end  
        end 
        
        if ~found_copy
            pars = struct;
            pars.filename = modFileNames{i};
            pars.varname = {};
            pars.varvalue = {};
            arr_idx = arr_idx + 1;
            arr_pars{arr_idx, 1} = pars;
        end   
    end
    
    if ~isempty(eval(['modParams_', neuronType]))
        file_names = fieldnames(eval(['modParams_', neuronType]));
        
        for i = 1 : length(file_names)
            pars = struct;
            pars.filename = file_names{i};
            var_names = eval(['modParams_', neuronType, '.', file_names{i}]);
            var_names = fieldnames(var_names);
            pars.varname = var_names;
            var_value = cell(length(var_names), 1);
            for j = 1 : length(var_names)
                var_value{j} = eval(['modParams_', neuronType, '.', file_names{i}, '.', var_names{j}]);
            end
            
            pars.varvalue = var_value;
            arr_idx = arr_idx + 1;
            arr_pars{arr_idx, 1} = pars;
        end
    end 
end

