function AddNumericVector(name, string, unit, desc,   relPred, valPred)
% Add numeric vector

    if nargin < 5
        % The parameter is always relevant
        relPred = 'true';
    end
    
    % Extend the validation predicate
    valPred_ = sprintf('isnumeric(%s) && all(~isnan(%s)) && all(~isinf(%s))', name, name, name);
    if nargin == 6
        valPred_ = [valPred_, ' && ', valPred];
    end
    
    % The name
    [handlers, xPos] = CreateParamNameText(name);
    
    % The control itself
    [handlers(end + 1), xPos] = CreateParamEditBox(string, xPos, desc);
    
    % The unit
    if ~isempty(unit)
        handlers(end + 1) = CreateParamUnitText(unit, xPos);
    end
    
    CommitParam(name, string, relPred, valPred_, handlers, unit);
    
end