function generic_Callback(hObject, ~)
% Callback function for editbox, checkbox, popupmenu and uitable

    global params
    
    userData = get(hObject, 'UserData');
    panIdx = userData(1);
    parIdx = userData(2);
    
    try
        style = get(hObject, 'Style');
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
    
    params{panIdx}{parIdx}.value = get(hObject, field);
    
    % The main controls
    UpdateViewControls();
    
    % Slider and gray strips
    AdjustSliderAndStrips(false);
    
end
