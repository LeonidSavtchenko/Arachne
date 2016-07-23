function AddPosIntScalar(name, value, desc,   relPred, valPred)
%% Add positive integer scalar

    if nargin < 4
        relPred = 'true';
    end
    
    valPred_ = [name, ' > 0'];
    if nargin == 5
        valPred_ = [valPred_, ' && ', valPred];
    end
    
    AddIntScalar(name, value, desc, relPred, valPred_);
    
end