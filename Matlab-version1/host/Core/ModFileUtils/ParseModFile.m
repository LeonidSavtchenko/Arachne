function [blocks, freeLocalVars] = ParseModFile(path)

    mod_keywords = {'TITLE', 'NEURON', 'CONSTANT', 'PARAMETER', 'UNITS', 'ASSIGNED', 'STATE', 'BREAKPOINT', 'INITIAL', 'FUNCTION', 'DERIVATIVE', 'PROCEDURE', 'KINETIC', 'DEFINE'};
 
    mod_file = fopen(path);
    if mod_file == -1
        error('Cannot open file for reading');
    end

    blocks = RunIncludePreprocessor(path, mod_keywords);
    
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

                rigth_border = 1;
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
                    field = blocks.(mod_keywords{idx});
                    field{num_blocks(idx)} = clean_lines;
                    blocks.(mod_keywords{idx}) = field;
                else
                    blocks.(mod_keywords{idx}) = clean_lines;
                end
                
            end
        end
        if ~found
            line = fgetl(mod_file);
        end
    end   
    fclose(mod_file);
    
    freeLocalVars = GetFreeLocalVars(path);
    
end

function blocks = RunIncludePreprocessor(path, mod_keywords)

    modFile = fopen(path, 'r');
    if modFile == -1
        error('Cannot open file for reading');
    end
    
    blocks = struct();
    for idx = 1 : length(mod_keywords)
        blocks.(mod_keywords{idx}) = {};
    end
    
    lines = {};
    while ~feof(modFile)
        
        line = fgetl(modFile);
        keywords_idx = -1; %#ok<NASGU>
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
                            keywords_idx = idx; %#ok<NASGU>
                            found = true;
                            break
                        end
                    end
                end
                
                if found
                    lines{end + 1, 1} = line; %#ok<AGROW>
                end
            end 
            
            if strcmp(mod_keywords{idx}, 'PROCEDURE') || strcmp(mod_keywords{idx}, 'FUNCTION')
                field = blocks.(mod_keywords{idx});
                field{end + 1} = lines;
                blocks.(mod_keywords{idx}) = field;
            else
                blocks.(mod_keywords{idx}) = lines;
            end
    
            lines = {};
            fclose(include_file);
        end
    end 
    fclose(modFile);
end

function vars = GetFreeLocalVars(path)
    
    vars = {};
    modFile = fopen(path, 'r');
    if modFile == -1
        error('Cannot open file for reading');
    end
    
    checksum = 0;
    isComment = false;
    while ~feof(modFile)

        line = fgetl(modFile);
        
        if strcmp(line, 'COMMENT')
            isComment = true;
        elseif strcmp(line, 'ENDCOMMENT')
            isComment = false;
            continue;
        end
        
        if isComment
            continue
        end
        
        leftScope = strfind(line, '{');  
        if ~isempty(leftScope)
            checksum =  checksum + length(leftScope);
        end

        rightScope = strfind(line, '}');
        if ~isempty(rightScope)
            checksum =  checksum - length(rightScope);
        end
              
        if checksum == 0 &&  ~isempty(strfind(line, 'LOCAL'));  
            vars{end + 1, 1} = line; %#ok<AGROW>
        end    
    end
    
    if isempty(vars)
        return
    end
    
    for i = 1 : length(vars)
        tmpStr = strsplit(vars{i}, ':');
        vars{i} = [strtrim(regexprep(tmpStr{1}, 'LOCAL', 'T')), ';']; %#ok<AGROW>
    end
end