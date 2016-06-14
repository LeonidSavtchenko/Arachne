function AddCustomCodeTextArea(name, text, desc,   relPred, valPred)
%% Add text area for custom m-code

    if nargin < 4
        relPred = 'true';
    end
    
    % Make sure that evaluation of the code does not lead to an error
    valPred_ = sprintf('evalTextArea(%s)', name);
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
    [handlers(end + 1), ~] = CreateMultiLineEditBox(text, desc, xPos);
    
    evalTextArea(text);
    
    CommitParam(name, text, relPred, valPred_, handlers, '');

end

function [hc, xPos] = CreateMultiLineEditBox(text, desc, xPos)

    global panIdx params layout palette
    
    xPos = xPos + layout.xMargin2;
    parIdx = length(params{panIdx}) + 1;
    
    hc = uicontrol('Style', 'edit', ...
                   'Units', 'pixels', ...
                   'Position', [xPos, 0, layout.ebWidth, layout.taHeight], ...
                   'String', text, ...
                   'UserData', [panIdx, parIdx], ...
                   'BackgroundColor', palette.validColor, ...
                   'Callback', @textArea_callback, ...
                   'TooltipString', desc, ...
                   'Max', 2, ...
                   'HorizontalAlignment', 'left');
    xPos = xPos + layout.ebWidth;
    
end

function textArea_callback(hObject, ~)
    
    text = get(hObject, 'String');
    evalTextArea(text);
    generic_Callback(hObject);
    
end