function AddIntVector(name, string, desc,   relPred, valPred)
%% Add integer vector

    if nargin < 4
        relPred = 'true';
    end
    
    valPred_ = sprintf('all(rem(%s, 1) == 0)', name);
    if nargin == 5
        valPred_ = [valPred_, ' && ', valPred];
    end
    
    unit = 'tg';
    AddNumericVector(name, string, unit, desc, relPred, valPred_);
    
end
