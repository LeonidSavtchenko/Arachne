function LoadParams(filename)

global params

% Read data from the file
input = load(filename);

% Check if params are saved as structure
guiParamsExist = any(strcmp(fieldnames(input), 'guiParams'));

if guiParamsExist
    input = input.guiParams;
end

%% Update values of the parameters in the GUI

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
        
        controlIsTextArea = false;
        
        if strcmp(style, 'edit')
            field = 'String';
            min = get(handlers(2), 'Min');
            max = get(handlers(2), 'Max');
            if max - min > 1
                controlIsTextArea = true;
            end
        elseif strcmp(style, 'checkbox') || strcmp(style, 'popupmenu')
            field = 'Value';
        elseif strcmp(style, 'uitable')
            field = 'Data';
        else
            assert(false);
        end
        
        oldvalue = get(handlers(2), field);        
        try
            cmd = sprintf('value = input.%s;', varname);
            eval(cmd);
        catch
            value = oldvalue;
        end
        
        set(handlers(2), field, value);
        params{panIdx_}{parIdx}.value = value;
        
        if controlIsTextArea
           evalTextArea(value); 
        end
    end
end

end
