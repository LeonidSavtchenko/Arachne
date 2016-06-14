function AddStringScalar(name, string, desc,   relPred, valPred, ext)
%% Add string vector

    if nargin < 4
        % The parameter is always relevant
        relPred = 'true';
    end
    
    % The validation predicate
    if nargin < 5
        valPred = 'true';
    end
    
    % The extension
    if nargin < 6
        ext = '';
    end
    
    % The name
    [handlers, xPos] = CreateParamNameText(name);
    
    % The control itself
    [handlers(end + 1), xPos] = CreateParamEditBox(string, xPos, desc);
    
    % The extension
    if ~isempty(ext)
        handlers(end + 1) = CreateParamUnitText(ext, xPos);
    end
    
    CommitParam(name, string, relPred, valPred, handlers, ext);
    
end