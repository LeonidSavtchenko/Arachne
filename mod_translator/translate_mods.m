function translate_mods(paths_to_mod, out_path)

    for i = 1 : length(paths_to_mod)
        path_to_mod = paths_to_mod{i};
        [~, mod_file_name, ~] = fileparts(path_to_mod);
        
        fprintf('Generating %s.h ...\n', mod_file_name);
        parameters_blocks = mod_file_parser(path_to_mod);
        mod_to_h(mod_file_name, parameters_blocks, out_path);
        
        fprintf('Generating %s.cpp ...\n', mod_file_name);
        mod_to_cpp(mod_file_name, parameters_blocks, out_path);
    end
    
end

function parameters_blocks = mod_file_parser(path)

    mod_keywords = {'TITLE', 'NEURON', 'PARAMETER', 'UNITS', 'ASSIGNED', 'STATE', 'BREAKPOINT', 'INITIAL', 'FUNCTION', 'DERIVATIVE', 'PROCEDURE'};

    mod_file = fopen(path);
    if mod_file == -1
        error('Cannot open file for reading');
    end

    parameters_blocks = struct();
    for idx = 1 : length(mod_keywords)
        parameters_blocks = setfield(parameters_blocks, mod_keywords{idx}, {});
    end

    num_blocks = zeros(size(mod_keywords));

    line = fgetl(mod_file);

    while ischar(line) || isempty(line)

        found = false;
        for idx = 1 : length(mod_keywords)

            if strfind(line, mod_keywords{idx})

                found = true;

                lines = {};
                lines{end + 1, 1} = line;

                line = fgetl(mod_file);

                while ischar(line) || isempty(line)

                    contain_keyword = false;

                    for j = 1 : length(mod_keywords)
                        if ~isempty(strfind(line, mod_keywords{j})) && isempty(strfind(line, 'UNITSOFF'))
                            contain_keyword = true;
                            break
                        end
                    end

                    if contain_keyword
                        break
                    end

                    lines{end + 1, 1} = line; %#ok<AGROW>

                    line = fgetl(mod_file);

                end

                num_blocks(idx) = num_blocks(idx) + 1;

                rigth_border = -1;
                len_lines = length(lines);
                for k = 0 : len_lines - 1
                    if strfind(lines{len_lines - k}, '}')
                        rigth_border = len_lines - k;
                        break
                    end
                end

                clean_lines = {};

                for k = 1 : rigth_border
                    clean_lines{end + 1, 1} = lines{k};
                end

                if strcmp(mod_keywords{idx}, 'PROCEDURE') || strcmp(mod_keywords{idx}, 'FUNCTION')
                    field = getfield(parameters_blocks, mod_keywords{idx});
                    field{num_blocks(idx)} = clean_lines;
                    parameters_blocks = setfield(parameters_blocks, mod_keywords{idx}, field);
                else
                    parameters_blocks = setfield(parameters_blocks, mod_keywords{idx}, clean_lines);
                end
                
            end
        end
        if ~found
            line = fgetl(mod_file);
        end
    end

    fclose(mod_file);
end
