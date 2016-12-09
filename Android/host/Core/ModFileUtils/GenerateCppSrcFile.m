function GenerateCppSrcFile(mod_file_name, parameters_blocks, out_path)

    cppTemplatePath = fullfile(cd, 'Core', 'ModFileUtils', 'CppCodeTemplates', 'SrcTemplate.cpp');

    fCpp = fopen(cppTemplatePath, 'r');

    if fCpp == -1
        error('Cannot open file for writing');
    end

    cppFile = {};
    tline = fgetl(fCpp);
    cppFile{end + 1, 1} = tline;

    while true

        tline = fgetl(fCpp);

        if tline == -1
            break
        end

        if strfind(tline, '%INCLUDE_HEADER')

            cppFile{end + 1, 1} = ['#include "', mod_file_name, '.h"']; %#ok<AGROW>

        elseif strfind(tline, '%CTOR_SIGNATURE')

            cppFile{end + 1, 1} = [mod_file_name, '<T>::', mod_file_name, '()']; %#ok<AGROW>

        elseif strfind(tline, '%INIT_CURRENTS')
            
            assigned_params = ParseAssignedBlock(parameters_blocks.ASSIGNED);
            parameter_params = ParseParameterBlock(parameters_blocks.PARAMETER);

            notInitCurrentParams = {};

            for i = 1 : length(assigned_params)
                tmpStr = assigned_params{i};

                if strfind(tmpStr, '=')
                    continue
                end

                if tmpStr(1) == 'i'
                    notInitCurrentParams{end + 1, 1} = assigned_params{i}; %#ok<AGROW>
                end
            end
            
            for i = 1 : length(parameter_params)
                tmpStr = parameter_params{i};

                if strfind(tmpStr, '=')
                    continue
                end

                if tmpStr(1) == 'i'
                    notInitCurrentParams{end + 1, 1} = parameter_params{i}; %#ok<AGROW>
                end
            end

            cppFile{end + 1, 1} = '    this->_currents = std::vector<T*>(_numCurrents);'; %#ok<AGROW>
           
            countCurrents = -1;
            for i = 1 : length(notInitCurrentParams)
                if isempty(strfind(notInitCurrentParams{i}, '['))
                    countCurrents = countCurrents + 1;
                    cppFile{end + 1, 1} = ...
                        ['    this->_currents[', int2str(countCurrents), '] = &', notInitCurrentParams{i}, ';']; %#ok<AGROW>
                end
            end

        elseif strfind(tline, '%INIT_STATES')

            scope = parameters_blocks.STATE;
            if ~isempty(scope)
                stateParams = ParseStateBlock(scope);

                if ~isempty(stateParams)
                    cppFile{end + 1, 1} = '    this->_states = std::vector<T*>(_numStates);'; %#ok<AGROW>
                end
                 countStates = -1;
                for i = 1 : length(stateParams)
                     if isempty(strfind(stateParams{i}, '['))
                         countStates = countStates + 1;
                         cppFile{end + 1, 1} = ...
                             ['    this->_states[', int2str(countStates), '] = &', stateParams{i}, ';']; %#ok<AGROW>
                     end
                end
            end
            
        elseif strfind(tline, '%INIT_DERIVATIVES')
            
            scope = parameters_blocks.STATE;
            if ~isempty(scope)
                stateParams = ParseStateBlock(scope);
                
                if ~isempty(stateParams)
                    cppFile{end + 1, 1} = '    this->_derivatives = std::vector<T*>(_numStates);'; %#ok<AGROW>
                end
                countStates = -1;
                for i = 1 : length(stateParams)
                    if isempty(strfind(stateParams{i}, '['))
                        countStates = countStates + 1;
                        cppFile{end + 1, 1} = ...
                            ['    this->_derivatives[', int2str(countStates), '] = &', stateParams{i}, '_rhp;']; %#ok<AGROW>
                    end
                end
            end
            
        elseif strfind(tline, '%INITIAL_SIGNATURE')

            cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::init()']; %#ok<AGROW>

        elseif strfind(tline, '%INITIAL_BODY')
            
            params = parameters_blocks.INITIAL;
            
            i = 1;
            while i <= length(params)
                
                cStr = strtrim(params{i});
                
                if strfind(cStr, 'INITIAL')
                    
                    cppFile{end + 1, 1} = '{'; %#ok<AGROW>
                    i = i + 1;
                    
                elseif strcmp(cStr, 'VERBATIM')
          
                    i = i + 1;
                    cStr = strtrim(params{i});
                    
                    while ~strcmp(cStr, 'ENDVERBATIM')
                        cppFile{end + 1, 1} = cStr; %#ok<AGROW>
                        i = i + 1;
                        cStr = strtrim(params{i});
                    end
                    i = i + 1;
                else
                    cStr = strsplit(cStr, ':');
                    cStr = strtrim(cStr{1});
                    cStr = TranslateLineOfCode(cStr);
                    
                    if isempty(cStr)
                        cppFile{end + 1, 1} = ''; %#ok<AGROW>
                    elseif strfind(cStr, 'LOCAL')
                        cppFile{end + 1, 1} = [regexprep(cStr, 'LOCAL', 'T'), ';']; %#ok<AGROW>
                        
                    elseif strfind(cStr, 'FROM')
                        
                        cppFile{end + 1, 1} = ParsForSycleHdr(cStr); %#ok<AGROW>
                        
                    elseif strfind(cStr, '=')
                        if cStr(end) == '}'
                            cppFile{end + 1, 1} = [cStr(1 : end - 1), ';}']; %#ok<AGROW>
                        elseif cStr(end) == '{'
                            cppFile{end + 1, 1} = cStr; %#ok<AGROW>   
                        else
                            cppFile{end + 1, 1} = [cStr, ';']; %#ok<AGROW>
                        end
                    else
                        if cStr(end) == ')'
                            cppFile{end + 1, 1} = [cStr, ';']; %#ok<AGROW>
                        else
                            cppFile{end + 1, 1} = cStr; %#ok<AGROW>
                        end
                    end
                    
                    i = i + 1; 
                end
            end

        elseif strfind(tline, '%BREAKPOINT_SIGNATURE')

            cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::currents()']; %#ok<AGROW>

        elseif strfind(tline, '%BREAKPOINT_BODY')

            params = parameters_blocks.BREAKPOINT;
            
            i = 1;
            while i <= length(params)
                
                cStr = strtrim(params{i});
                
                if strfind(cStr, 'BREAKPOINT') 
                    cppFile{end + 1, 1} = '{'; %#ok<AGROW>
                    i = i + 1;
                elseif isempty(cStr) || ~isempty(strfind(cStr, 'SOLVE')) || strcmp(cStr(1), ':')
                    i = i + 1;
                    continue
                elseif strfind(cStr, 'LOCAL')
                    i = i + 1;
                    cppFile{end + 1, 1} = [regexprep(cStr, 'LOCAL', 'T'), ';']; %#ok<AGROW>
                elseif strcmp(cStr, 'VERBATIM')
                    i = i + 1; 
                    cStr = strtrim(params{i});
                           
                    while ~strcmp(cStr, 'ENDVERBATIM')
                        cppFile{end + 1, 1} = cStr; %#ok<AGROW>
                        i = i + 1; 
                        cStr = strtrim(params{i});
                    end               
                    i = i + 1; 
                elseif strfind(cStr, '=')
                    i = i + 1;
                    cppFile{end + 1, 1} = [TranslateLineOfCode(cStr), ';']; %#ok<AGROW>
                elseif isempty(cStr) && cStr(1) == ':'
                    i = i + 1;
                    cppFile{end + 1, 1} = ['//', cStr(2 : end)]; %#ok<AGROW>
                else
                    i = i + 1;
                    cppFile{end + 1, 1} = cStr(); %#ok<AGROW>
                end
            end
            
        elseif strfind(tline, '%DERIVATIVE_SIGNATURE')

            cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::states()']; %#ok<AGROW>

        elseif strfind(tline, '%DERIVATIVE_BODY')

            params = parameters_blocks.DERIVATIVE;

            for i = 2 : length(params) - 1
                if strfind(params{i}, '=')
                    cline = regexprep(params{i}, '\s*', '');
                    line = ['    ', regexprep(cline, '''', '_rhp'), ';'];
                else
                    line = ['    ', regexprep(params{i}, '\s*', ''), ';'];
                end
                cppFile{end + 1, 1} = TranslateLineOfCode(line); %#ok<AGROW>
            end

        elseif strfind(tline, '%PROCEDURE_DEFINITIONS')

            for idx = 1 : length(parameters_blocks.PROCEDURE)

                proc = parameters_blocks.PROCEDURE{idx};
                [name, formal_params, proc_body] = ParseProcedureBlock(proc);

                cppFile{end + 1, 1} = ''; %#ok<AGROW>
                cppFile{end + 1, 1} = 'template <typename T>'; %#ok<AGROW>
                
                if isempty(formal_params{1})
                    cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::', name, '()']; %#ok<AGROW>
                else 
                    var_names = '';
                    for i = 1 : length(formal_params) - 1
                        var_names = [var_names, 'T ', formal_params{i}, ', ']; %#ok<AGROW>
                    end
                    var_names = [var_names, 'T ', formal_params{end}]; %#ok<AGROW>
                    cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::', name, '(', var_names, ')']; %#ok<AGROW>
                end
                      
                cppFile{end + 1, 1} = '{'; %#ok<AGROW>

                for i = 1 : length(proc_body)

                    tmpStr = strsplit(proc_body{i}, ':');
                    tmpStr = tmpStr{1};
                    tmpStr = TranslateLineOfCode(tmpStr);
                    
                    if isempty(tmpStr) || ~isempty(strfind(tmpStr, 'PROCEDURE')) || ...
                            ~isempty(strfind(tmpStr, 'TABLE')) || ...
                            ~isempty(strfind(tmpStr, 'UNITSOFF'))
                        
                        continue
                    elseif strfind(tmpStr, 'LOCAL')
                        cppFile{end + 1, 1} = [regexprep(tmpStr, 'LOCAL', 'T'), ';']; %#ok<AGROW>
                        
                    elseif strfind(tmpStr, 'FROM')

                        cppFile{end + 1, 1} = ParsForSycleHdr(tmpStr); %#ok<AGROW>
                        
                    elseif strfind(tmpStr, '=')
                        if tmpStr(end) == '}'
                            cppFile{end + 1, 1} = [tmpStr(1 : end - 1), ';}']; %#ok<AGROW>
                        elseif cStr(end) == '{'
                            cppFile{end + 1, 1} = cStr; %#ok<AGROW>   
                        else
                            cppFile{end + 1, 1} = [tmpStr, ';']; %#ok<AGROW>
                        end
                    else
                        cppFile{end + 1, 1} = tmpStr; %#ok<AGROW>
                    end
                end
            end 
         
        elseif strfind(tline, '%FUNCTION_DEFINITIONS')
            
            for idx = 1 : length(parameters_blocks.FUNCTION)
                
                func = parameters_blocks.FUNCTION{idx};
                [name, formal_params, func_body] = ParseFunctionBlock(func);

                cppFile{end + 1, 1} = ''; %#ok<AGROW>
                cppFile{end + 1, 1} = 'template <typename T>'; %#ok<AGROW>
                
                if isempty(formal_params{1})
                    cppFile{end + 1, 1} = ['T ', mod_file_name, '<T>::', name, '()']; %#ok<AGROW>
                else 
                    var_names = '';
                    for i = 1 : length(formal_params) - 1
                        var_names = [var_names, 'T ', formal_params{i}, ', ']; %#ok<AGROW>
                    end
                    var_names = [var_names, 'T ', formal_params{end}]; %#ok<AGROW>
                    cppFile{end + 1, 1} = ['T ', mod_file_name, '<T>::', name, '(', var_names, ')']; %#ok<AGROW>
                end
                      
                cppFile{end + 1, 1} = '{'; %#ok<AGROW>
                cppFile{end + 1, 1} = ['T ', '_', name, ';']; %#ok<AGROW>

                for i = 1 : length(func_body)

                    tmpStr = strsplit(func_body{i}, ':');
                    tmpStr = tmpStr{1};
                    tmpStr = TranslateLineOfCode(tmpStr);
                    
                    if isempty(tmpStr) || ~isempty(strfind(tmpStr, 'FUNCTION')) || ...
                            ~isempty(strfind(tmpStr, 'TABLE')) || ...
                            ~isempty(strfind(tmpStr, 'UNITSOFF'))
                        
                        continue
                    elseif strfind(tmpStr, 'LOCAL')
                        cppFile{end + 1, 1} = [regexprep(tmpStr, 'LOCAL', 'T'), ';']; %#ok<AGROW>
                        
                    elseif strfind(tmpStr, 'FROM')

                        cppFile{end + 1, 1} = ParsForSycleHdr(tmpStr); %#ok<AGROW>
                        
                    elseif strfind(tmpStr, '=')
                        if strfind(tmpStr, name)
                            tmpStr = regexprep(tmpStr, name, ['_', name]);
                        end
                        if tmpStr(end) == '}'
                            cppFile{end + 1, 1} = [tmpStr(1 : end - 1), ';}']; %#ok<AGROW>
                        elseif cStr(end) == '{'
                            cppFile{end + 1, 1} = cStr; %#ok<AGROW>   
                        else
                            cppFile{end + 1, 1} = [tmpStr, ';']; %#ok<AGROW>
                        end
                    elseif i == length(func_body)
                        cppFile{end + 1, 1} = ['    return _', name, ';']; %#ok<AGROW>
                        cppFile{end + 1, 1} = tmpStr; %#ok<AGROW>
                    else
                        cppFile{end + 1, 1} = tmpStr; %#ok<AGROW>
                    end
                end
            end 
            
        elseif strfind(tline, '%TEMPLATE_INSTANTIATION_FLOAT')

            cppFile{end + 1, 1} = ['class ', mod_file_name, '<float>;']; %#ok<AGROW>

        elseif strfind(tline, '%TEMPLATE_INSTANTIATION_DOUBLE')

            cppFile{end + 1, 1} = ['class ', mod_file_name, '<double>;']; %#ok<AGROW>
            
        else
            
            cppFile{end + 1, 1} = tline; %#ok<AGROW>
            
        end
    end
        
    fclose(fCpp);

    cppPath = fullfile(out_path, [mod_file_name, '.cpp']);

    fid = fopen(cppPath, 'w');

    if fid == -1
        error('Cannot open file for writing');
    end

    for i = 1 : length(cppFile)
        fprintf(fid, '%s \r\n', cppFile{i});
    end

    fclose(fid);
end

function outStr = ParsForSycleHdr(inStr)

    leftIdx = strfind(inStr, 'FROM');
    leftIdx = leftIdx(1) + 5;
    rigthIdx = strfind(inStr, '=');
    rigthIdx = rigthIdx(1) - 1;
    counterName = strtrim(inStr(leftIdx : rigthIdx));

    leftIdx = rigthIdx + 2;
    rigthIdx = strfind(inStr, 'TO');
    rigthIdx = rigthIdx(1) - 1;
    startValue = strtrim(inStr(leftIdx : rigthIdx));

    leftIdx = rigthIdx + 3;
    rigthIdx = strfind(inStr, '{');
    rigthIdx = rigthIdx(1) - 1;
    endValue = strtrim(inStr(leftIdx : rigthIdx));

    outStr = ['for (int ', counterName, ' = ', startValue, ...
        '; ', counterName, ' <= ', endValue, ';', ...
        ' ++', counterName, ') {'];
    
    outStr = regexprep(outStr, '\(T\)', '');
end
