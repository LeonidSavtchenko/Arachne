function AddRatVector(name, value, unit, desc,   relPred, valPred)

    if nargin < 5
        % The parameter is always relevant
        relPred = 'true';
    end
    
    if nargin < 6
        % The parameter is always valid
        valPred = 'true';
    end
    
    AddNumericVector(name, value, unit, desc, relPred, valPred);
end