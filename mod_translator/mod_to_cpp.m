function mod_to_cpp(mod_file_name, parameters_blocks, out_path)

    cppTemplatePath = fullfile(cd, 'templates', 'AutogenCodeTemplate.cpp');

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

            params = parameter_parser(parameters_blocks.PARAMETER);

            cline = '';

            for i = 1 : length(params)

                if strfind(params{i}, 'v')
                    continue
                end

                if isempty(strfind(params{i}, '='))
                    cline = [cline, ', T ', params{i}];
                end
            end

            cppFile{end + 1, 1} = [mod_file_name, '<T>::', mod_file_name, '(T v', cline, ')']; %#ok<AGROW>

        elseif strfind(tline, '%INIT_CURRENTS')

            assigned_params = assigned_parser(parameters_blocks.ASSIGNED);
            parameter_params = parameter_parser(parameters_blocks.PARAMETER);

            not_init_params = {};

            for i = 1 : length(parameter_params)
                curr_str = parameter_params{i};

                if isempty(strfind(curr_str, '='))
                    continue
                end

                if curr_str(1) == 'i'
                    not_init_params{end + 1, 1} = parameter_params{i}; %#ok<AGROW>
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

            for i = 1 : length(current_params)
                cppFile{end + 1, 1} = ['    this->_currents[', int2str(i - 1), '] = &', current_params{i}, ';']; %#ok<AGROW>
            end

        elseif strfind(tline, '%INIT_STATES')

            scope = parameters_blocks.STATE;
            if ~isempty(scope)
                state_params = state_parser(scope);

                for i = 1 : length(state_params)
                    cppFile{end + 1, 1} = ['    this->_states[', int2str(i - 1), '] = &', state_params{i}, ';']; %#ok<AGROW>
                end
            end
            
        elseif strfind(tline, '%INIT_DERIVATIVES')

            scope = parameters_blocks.STATE;
            if ~isempty(scope)
                state_params = state_parser(scope);

                for i = 1 : length(state_params)
                    cppFile{end + 1, 1} = ['    this->_derivatives[', int2str(i - 1), '] = &', state_params{i}, '_rhp;']; %#ok<AGROW>
                end
            end

        elseif strfind(tline, '%INITIAL_SIGNATURE')

            cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::init()']; %#ok<AGROW>

        elseif strfind(tline, '%INITIAL_BODY')

            params = parameters_blocks.INITIAL;

            for i = 2 : length(params) - 1
                cppFile{end + 1, 1} = ['    ', regexprep(params{i}, '\s*', ''), ';']; %#ok<AGROW>
            end

        elseif strfind(tline, '%BREAKPOINT_SIGNATURE')

            cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::currents()']; %#ok<AGROW>

        elseif strfind(tline, '%BREAKPOINT_BODY')

            params = parameters_blocks.BREAKPOINT;

            for i = 1 : length(params)
                if strfind(params{i}, '=')
                    cppFile{end + 1, 1} = ['    ', regexprep(params{i}, '\s*', ''), ';']; %#ok<AGROW>
                end
            end

        elseif strfind(tline, '%DERIVATIVE_SIGNATURE')

            cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::states()']; %#ok<AGROW>

        elseif strfind(tline, '%DERIVATIVE_BODY')

            params = parameters_blocks.DERIVATIVE;

            for i = 2 : length(params) - 1
                if strfind(params{i}, '=')
                    cline = regexprep(params{i}, '\s*', '');
                    cppFile{end + 1, 1} = ['    ', regexprep(cline, '''', '_rhp'), ';']; %#ok<AGROW>
                else
                    cppFile{end + 1, 1} = ['    ', regexprep(params{i}, '\s*', ''), ';']; %#ok<AGROW>
                end
            end

        elseif strfind(tline, '%PROCEDURE_DEFINITIONS')

            for idx = 1 : length(parameters_blocks.PROCEDURE)

                proc = parameters_blocks.PROCEDURE{idx};
                [name, formal_params, proc_body] = procedure_parser(proc);

                cppFile{end + 1, 1} = ''; %#ok<AGROW>
                cppFile{end + 1, 1} = 'template <typename T>'; %#ok<AGROW>

                var_names = '';
                for i = 1 : length(formal_params) - 1
                    var_names = [var_names, 'T ', formal_params{i}, ', '];
                end
                var_names = [var_names, 'T ', formal_params{end}];
                cppFile{end + 1, 1} = ['void ', mod_file_name, '<T>::', name, '(', var_names, ')']; %#ok<AGROW>
                cppFile{end + 1, 1} = '{'; %#ok<AGROW>

                for i = 1 : length(proc_body)

                    proc_body{i} = translate_line_of_code(proc_body{i});

                    if strfind(proc_body{i}, 'LOCAL')
                        cppFile{end + 1, 1} = '    // LOCAL'; %#ok<AGROW>
                        cppFile{end + 1, 1} = [regexprep(proc_body{i}, 'LOCAL', 'T'), ';']; %#ok<AGROW>
                        cppFile{end + 1, 1} = ''; %#ok<AGROW>
                    elseif strfind(proc_body{i}, ':')
                        if ~(isempty(strfind(proc_body{i}, 'PROCEDURE')) || isempty(strfind(proc_body{i}, ':')))
                            cline = strsplit(proc_body{i}, ':');
                            cppFile{end + 1, 1} = ['//', cline{2}]; %#ok<AGROW>
                        else
                            cppFile{end + 1, 1} = regexprep(proc_body{i}, ':', '//'); %#ok<AGROW>
                        end
                    elseif strfind(proc_body{i}, '=')
                        if isempty(strfind(proc_body{i}, '}'))
                            cppFile{end + 1, 1} = [proc_body{i}, ';']; %#ok<AGROW>
                        else
                            cppFile{end + 1, 1} = regexprep(proc_body{i}, '}', ';}'); %#ok<AGROW>
                        end
                    elseif ~(isempty(strfind(proc_body{i}, 'if')) && isempty(strfind(proc_body{i}, 'else')))
                        if isempty(strfind(proc_body{i}, '}'))
                            cppFile{end + 1, 1} = proc_body{i}; %#ok<AGROW>
                        else
                            cppFile{end + 1, 1} = regexprep(proc_body{i}, '}', ';}'); %#ok<AGROW>
                        end
                    end
                end
                cppFile{end + 1, 1} = '}'; %#ok<AGROW>
            end

        elseif strfind(tline, '%FUNCTION_DEFINITIONS')

            for idx = 1 : length(parameters_blocks.FUNCTION)

                func = parameters_blocks.FUNCTION{idx};
                [name, formal_params, func_body] = function_parser(func);

                cppFile{end + 1, 1} = ''; %#ok<AGROW>
                cppFile{end + 1, 1} = 'template <typename T>'; %#ok<AGROW>

                var_names = '';
                for i = 1 : length(formal_params) - 1
                    var_names = [var_names, 'T ', formal_params{i}, ', '];
                end
                var_names = [var_names, 'T ', formal_params{end}];
                cppFile{end + 1, 1} = ['T ', mod_file_name, '<T>::', name, '(', var_names, ')']; %#ok<AGROW>
                cppFile{end + 1, 1} = '{'; %#ok<AGROW>

                cppFile{end + 1, 1} = ['    T ', '_', name, ';']; %#ok<AGROW>

                for i = 1 : length(func_body) - 1

                    cline = func_body{i};

                    cline = translate_line_of_code(cline);
                    
                    if strfind(cline, ':')

                        if ~(isempty(strfind(cline, 'FUNCTION')) || isempty(strfind(cline, ':')))
                            cline = strsplit(cline, ':');
                            cppFile{end + 1, 1} = ['//', cline{2}]; %#ok<AGROW>
                        else
                            cppFile{end + 1, 1} = regexprep(cline, ':', '//'); %#ok<AGROW>
                        end

                    elseif strfind(cline, '=')
                        if strfind(cline, name)
                            cline = regexprep(cline, name, ['_', name]);
                        end

                        if isempty(strfind(cline, '}'))
                            cppFile{end + 1, 1} = [cline, ';']; %#ok<AGROW>
                        else
                            cppFile{end + 1, 1} = regexprep(cline, '}', ';}'); %#ok<AGROW>
                        end

                    elseif ~(isempty(strfind(cline, 'if')) && isempty(strfind(cline, 'else')))
                        if strfind(cline, name)
                            cline = regexprep(cline, name, ['_', name]);
                        end

                        if isempty(strfind(cline, '}'))
                            cppFile{end + 1, 1} = cline; %#ok<AGROW>
                        else
                            cppFile{end + 1, 1} = regexprep(cline, '}', ';}'); %#ok<AGROW>
                        end
                    elseif strfind(cline, '}')
                         cppFile{end + 1, 1} = cline; %#ok<AGROW>
                    end
                end

                cppFile{end + 1, 1} = ['    return ', ['_', name], ';']; %#ok<AGROW>
                cppFile{end + 1, 1} = '}'; %#ok<AGROW>
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

function out_line = translate_line_of_code(in_line)

    % Add "(T)" in front of all numbers
    
    expression1 = '(?<![a-zA-Z_])(?<![0-9])(([0-9]+)?\.)?[0-9]+([eE]([\+\-])?[0-9]+)?';
    replace1 = '(T)$&';
    out_line = regexprep(in_line, expression1, replace1);

    expression2 = '(?<=\d\.[Ee][\+\-])(\(T\)){1}';
    replace2 = '';
    out_line = regexprep(out_line, expression2, replace2);
    
    % Replace "^" operator with "pow" fincution call
    
    indexes = strfind(out_line, '^');

    if isempty(indexes)
        return
    else
        % TODO: !!
        warning('Replacement of "^" operator with "pow" funcution call is not implemented');
        out_line = strrep(out_line, '^', '*');
    end
end
