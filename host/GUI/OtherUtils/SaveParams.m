function SaveParams(filename)
%% Add all parameters to current workspace and save all necessary input data to file
    
    global params
    
    output = {};
    
    % Loop by panels
    for panIdx_ = 1 : length(params)
        
        % Loop by all parameters of this panel
        for parIdx = 1 : length(params{panIdx_})
            
            varname = params{panIdx_}{parIdx}.name;
            handlers = params{panIdx_}{parIdx}.handlers;
            
            try
                style = get(handlers(2), 'Style');
            catch
                style = 'uitable';
            end
            
            if strcmp(style, 'edit')
                field = 'String';
            elseif strcmp(style, 'checkbox') || strcmp(style, 'popupmenu')
                field = 'Value';
            elseif strcmp(style, 'uitable')
                field = 'Data';
            else
                assert(false);
            end
            
            value = get(handlers(2), field);
            
            if strcmp(style, 'edit') && size(value, 1) > 1
                % Join char array into the single string
                nRows = size(value, 1);
                str = strtrim(value(1, :));
                for row = 2 : nRows
                    str = sprintf('%s\n%s', str, strtrim(value(row, :)));
                end
                value = str; %#ok<NASGU>
            end
            
            if strcmp(style, 'checkbox')
                cmd = sprintf('%s = logical(value);', varname);
            elseif strcmp(style, 'popupmenu')
                cmd = sprintf('%s = int32(value);', varname);
            else
                cmd = sprintf('%s = value;', varname);
            end
            
            eval(cmd);
    
            output = [output; varname];
        end
    end
    
    save(filename, output{:});

end
