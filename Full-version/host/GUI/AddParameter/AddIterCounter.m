function AddIterCounter(name, value, desc,   relPred, valPred)
%% Add iteration counter parameter

    name = [name, 'Iter'];
    
    if nargin < 4
        relPred = 'true';
    end
    
    valPred_ = sprintf('rem(%s, 1) == 0 && %s > 0', name, name);
    if nargin == 5
        valPred_ = [valPred_, ' && ', valPred];
    end
        
    unit = 'it';
    AddRatScalar(name, value, unit, desc, relPred, valPred_);
    
end