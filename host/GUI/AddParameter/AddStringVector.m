function AddStringVector(name, string, desc,   relPred, valPred)
%% Add string vector

    if nargin < 4
        % The parameter is always relevant
        relPred = 'true';
    end
    
    % The validation predicate
    if nargin == 5
        valPred_ = valPred;
    else
        valPred_ = 'true';
    end
    
    % The name
    [handlers, xPos] = CreateParamNameText(name);
    
    % The control itself
    [handlers(end + 1), xPos] = CreateParamEditBox(string, xPos, desc);
    
    CommitParam(name, string, relPred, valPred_, handlers, '');
    
end