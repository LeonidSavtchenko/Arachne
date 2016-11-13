function blocks = ParseModFile(path)

    mod_keywords = {'TITLE', 'NEURON', 'CONSTANT', 'PARAMETER', 'UNITS', 'ASSIGNED', 'STATE', 'BREAKPOINT', 'INITIAL', 'FUNCTION', 'DERIVATIVE', 'PROCEDURE'};
 
    mod_file = fopen(path);
    if mod_file == -1
        error('Cannot open file for reading');
    end

    blocks = run_include_preprocessor(path, mod_keywords);
    
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
                    field = getfield(blocks, mod_keywords{idx});
                    field{num_blocks(idx)} = clean_lines;
                    blocks = setfield(blocks, mod_keywords{idx}, field);
                else
                    blocks = setfield(blocks, mod_keywords{idx}, clean_lines);
                end
                
            end
        end
        if ~found
            line = fgetl(mod_file);
        end
    end   
    fclose(mod_file);
    
end

function blocks = run_include_preprocessor(path, mod_keywords)

    mod_file = fopen(path, 'r');
    if mod_file == -1
        error('Cannot open file for reading');
    end
    
    blocks = struct();
    for idx = 1 : length(mod_keywords)
        blocks = setfield(blocks, mod_keywords{idx}, {});
    end
    
    lines = {};
    while ~feof(mod_file)
        
        line = fgetl(mod_file);
        keywords_idx = -1;
        if strfind(line, 'INCLUDE')           
            idx = strfind(line, '"');
            left_idx = idx(1) + 1;
            right_idx = idx(end) - 1;

            name_include_file = line(left_idx : right_idx);       
            idx = strfind(path, '\');
            path_to_include_mod = [path(1 : idx(end)), name_include_file];
            
            include_file = fopen(path_to_include_mod, 'r');
            if include_file == -1
                error('Cannot open file for reading');
            end
            
            found = false;
            while ~feof(include_file)
                
                line = fgetl(include_file);
 
                if ~found
                    for idx = 1 : length(mod_keywords)
                        if strfind(line, mod_keywords{idx})
                            keywords_idx = idx;
                            found = true;
                            break
                        end
                    end
                end
                
                if found
                    lines{end + 1, 1} = line;
                end
            end 
            
            if strcmp(mod_keywords{idx}, 'PROCEDURE') || strcmp(mod_keywords{idx}, 'FUNCTION')
                field = getfield(blocks, mod_keywords{idx});
                field{end + 1} = lines;
                blocks = setfield(blocks, mod_keywords{idx}, field);
            else
                blocks = setfield(blocks, mod_keywords{idx}, lines);
            end
    
            lines = {};
            fclose(include_file);
        end
    end 
    fclose(mod_file);
end