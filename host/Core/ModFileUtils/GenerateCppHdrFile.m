function GenerateCppHdrFile(mod_file_name, parameters_blocks, out_path, user_init_params)

    hTemplatePath = fullfile(cd, 'Core', 'ModFileUtils', 'CppCodeTemplates', 'HdrTemplate.h');

    fHeader = fopen(hTemplatePath, 'r');

    if fHeader == -1
        error('Cannot open file for writing');
    end

    headerFile = {};
    tline = fgetl(fHeader);
    headerFile{end + 1, 1} = tline;

    while true

        tline = fgetl(fHeader);

        if tline == -1
            break
        end

        if strfind(tline, '%CLASS_NAME')

            headerFile{end + 1, 1} = ['class ', mod_file_name, ' : public ModCurrentsBase<T>']; %#ok<AGROW>

        elseif strfind(tline, '%PROTOTYPE_DEFAULT_CONSTRUCTOR')
            
            headerFile{end + 1, 1} = ['    ', mod_file_name, '();']; %#ok<AGROW>
            
        elseif strfind(tline, '%CONSTANT_NAMES')
            
            params = ParseParameterBlock(parameters_blocks.CONSTANT);
            
            for i = 1 : length(params)
                str = strtrim(params{i});
                if ~isempty(strfind(str, '='))
                    headerFile{end + 1, 1} = strcat(['    const T ', str], ';'); %#ok<AGROW>
                end
            end
            
        elseif strfind(tline, '%PARAMETER_NAMES')

            params = ParseParameterBlock(parameters_blocks.PARAMETER);

            for i = 1 : length(params)

                str = strtrim(params{i});
                
                if strcmp(str, 'v')
                    continue
                end

                if isempty(strfind(str, '='))                 
                    var_names = user_init_params.varname;
                    var_values = user_init_params.varvalue;
                    for k = 1 : length(var_names)
                        if strcmp(str, var_names{k})           
                            if isnan(var_values{k})
                                headerFile{end + 1, 1} = strcat(['    const T ', str], ' =', ' std::numeric_limits<T>::quiet_NaN()',';'); %#ok<AGROW>
                            else
                                 headerFile{end + 1, 1} = strcat(['    const T ', str], ' = (T)', num2str(var_values{k}),';'); %#ok<AGROW>
                            end
                        end
                    end
                else
                    split_param = strsplit(str, '=');
                    headerFile{end + 1, 1} = strcat(['    const T ', split_param{1}], ' = (T)' , split_param{2}, ';'); %#ok<AGROW>
                end
            end

        elseif strfind(tline, '%STATE_NAMES')

            scope = parameters_blocks.STATE;
            if ~isempty(scope)
                params = ParseStateBlock(scope);

                cline = '    T ';
                len_params = length(params);
                for k = 1 : len_params - 1
                    cline = [cline, params{k}, ', '];
                end

                headerFile{end + 1, 1} = [cline, params{len_params}, ';']; %#ok<AGROW>
            end
            
        elseif strfind(tline, '%ASSIGNED_NAMES')

            params = ParseAssignedBlock(parameters_blocks.ASSIGNED);

            cline = '    T ';
            len_params = length(params);
            for k = 1 : len_params - 1
                cline = [cline, params{k}, ', '];
            end

            headerFile{end + 1, 1} = [cline, params{len_params}, ';']; %#ok<AGROW>

        elseif strfind(tline, '%DERIVATIVE_NAMES')

            scope = parameters_blocks.STATE;
            if ~isempty(scope)
                params = ParseStateBlock(scope);

                cline = '    T ';
                len_params = length(params);
                for k = 1 : len_params - 1
                    cline = [cline, params{k}, '_rhp, '];
                end

                headerFile{end + 1, 1} = [cline, params{len_params}, '_rhp;']; %#ok<AGROW>
            end
            
        elseif strfind(tline, '%PROCEDURE_NAMES')

            for idx = 1 : length(parameters_blocks.PROCEDURE)

                proc = parameters_blocks.PROCEDURE{idx};
                [name, formal_params] = ParseProcedureBlock(proc);

                var_names = '';
                for i = 1 : length(formal_params) - 1
                    var_names = [var_names, 'T ', formal_params{i}, ', '];
                end
                var_names = [var_names, 'T ', formal_params{length(formal_params)}];

                headerFile{end + 1, 1} = ['    void ', name, '(', var_names, ');']; %#ok<AGROW>
            end

        elseif strfind(tline, '%FUNCTION_NAMES')

            for idx = 1 : length(parameters_blocks.FUNCTION)

                func = parameters_blocks.FUNCTION{idx};
                [name, formal_params] = ParseFunctionBlock(func);

                var_names = '';
                for i = 1 : length(formal_params) - 1
                    var_names = [var_names, 'T ', formal_params{i}, ', '];
                end
                var_names = [var_names, 'T ', formal_params{length(formal_params)}];

                headerFile{end + 1, 1} = ['    T ', name, '(', var_names, ');']; %#ok<AGROW>
            end

        elseif strfind(tline, '%NUM_CURRENTS')

            assigned_params = ParseAssignedBlock(parameters_blocks.ASSIGNED);
            parameter_params = ParseParameterBlock(parameters_blocks.PARAMETER);

            not_init_params = {};

            for i = 1 : length(parameter_params)
                curr_str = parameter_params{i};

                if isempty(strfind(curr_str, '='))
                    continue
                end

                if curr_str(1) == 'i'
                    not_init_params{end + 1, 1} = parameter_params{i};
                end
            end

            current_params = {};

            if isempty(not_init_params)
                for j = 1 : length(assigned_params)
                    curr_str = assigned_params{j};
                    if curr_str(1) == 'i'
                         current_params{end + 1, 1} = curr_str;
                    end
                end
            else
                for i = 1 : length(not_init_params)
                    for j = 1 : length(assigned_params)

                        if ~strcmp(not_init_params{i}, assigned_params{j})
                            current_params{end + 1, 1} = assigned_params{j};
                        end

                    end
                end
            end
            
            if isempty(current_params)
                headerFile{end + 1, 1} = ['    const size_t _numCurrents = ', int2str(length(0)), ';']; %#ok<AGROW>
            else
                headerFile{end + 1, 1} = ['    const size_t _numCurrents = ', int2str(length(current_params)), ';']; %#ok<AGROW>
            end
            
        elseif strfind(tline, '%NUM_STATES')

            scope = parameters_blocks.STATE;
            if ~isempty(scope)
                params = ParseStateBlock(scope);
                
                if isempty(params)
                    headerFile{end + 1, 1} = ['    const size_t _numStates = ', int2str(0), ';']; %#ok<AGROW>
                else
                    headerFile{end + 1, 1} = ['    const size_t _numStates = ', int2str(length(params)), ';']; %#ok<AGROW>
                end
            end
        else
            headerFile{end + 1, 1} = tline; %#ok<AGROW>
        end

    end

    fclose(fHeader);

    hPath = fullfile(out_path, [mod_file_name, '.h']);

    fid = fopen(hPath, 'w');

    if fid == -1
        error('Cannot open file for writing');
    end

    for i = 1 : length(headerFile)
        fprintf(fid, '%s \r\n', headerFile{i});
    end

    fclose(fid);
end
