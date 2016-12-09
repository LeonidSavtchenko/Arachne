function AddModParamsTextArea(name, text, desc,   relPred, valPred)
%% Add text area for parameters of currents defined in MOD files

    if nargin < 4
        relPred = 'true';
    end
    
    % Make sure that evaluation of the code does not lead to an error
    valPred_ = sprintf('valPred_modParamsTextArea(%s, ''%s'')', name, name);
    if nargin == 5
        valPred_ = [valPred_, ' && ', valPred];
    end

    % The name
    [handlers, xPos] = CreateParamNameText(name);
    
    % Prepare tooltip
    if iscell(desc)
        desc = ['<html>', CellArray2HtmlString(desc), '</html>'];
    end

    % The control itself
    [handlers(end + 1), ~] = CreateMultiLineEditBox(text, name, desc, xPos);
    
    evalTextArea(text, name);
    
    CommitParam(name, text, relPred, valPred_, handlers, '');

end

function [hc, xPos] = CreateMultiLineEditBox(text, varName, desc, xPos)

    global panIdx params layout palette
    
    xPos = xPos + layout.xMargin2;
    parIdx = length(params{panIdx}) + 1;
    
    if strcmp(varName, 'modParams_e')
        i = 1;
    elseif strcmp(varName, 'modParams_i')
        i = 2;
    else
        i = -1;
    end
    
    hc = uicontrol('Style', 'edit', ...
                   'Units', 'pixels', ...
                   'Position', [xPos, 0, layout.ebWidth, layout.mptaHeight], ... 
                   'String', text, ...
                   'UserData', [panIdx, parIdx, i], ...
                   'BackgroundColor', palette.validColor, ...
                   'Callback', @textArea_callback, ...
                   'TooltipString', desc, ...
                   'Max', 2, ...
                   'HorizontalAlignment', 'left');
    xPos = xPos + layout.ebWidth;
end

function textArea_callback(hObject, ~)
    
    text = get(hObject, 'String');
    userData = get(hObject, 'UserData');
    i = userData(3);
    if i == 1
        varName = 'modParams_e';
    elseif i == 2
        varName = 'modParams_i';
    end
    evalTextArea(text, varName);
    generic_Callback(hObject);
    
end