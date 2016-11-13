function AddModFilesSelectorButton(name, text, desc,   relPred)
%% Add a pushbutton for selecting MOD files

    global layout palette
    global panIdx params
        
    if nargin == 3
        % The parameter is always relevant
        relPred = 'true';
    end
    
    % The name
    [handlers, xPos] = CreateParamNameText(name);
    
    % Prepare tooltip
    if iscell(desc)
        desc = ['<html>', CellArray2HtmlString(desc), '</html>'];
    end
    
    % The control itself
    xPos = xPos + layout.xMargin2;
    parIdx = length(params{panIdx}) + 1;    
    
    handlers(end + 1) = uicontrol('Style', 'pushbutton', ...
                                  'Units', 'pixels', ...
                                  'BackgroundColor', palette.backgroundColor, ...
                                  'Position', [xPos, 0, layout.ebWidth, layout.pbHeight], ...
                                  'String', text, ...
                                  'UserData', [panIdx, parIdx], ...
                                  'Callback', @modFilesSelector_Callback, ...
                                  'TooltipString', desc);   

    valPred = 'true'; % !!
    value = '';
    unit = '';
    CommitParam(name, value, relPred, valPred, handlers, unit);
    
end
