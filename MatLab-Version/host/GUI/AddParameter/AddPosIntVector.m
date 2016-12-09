function AddPosIntVector(name, string, desc,   relPred, valPred)
%% Add positive integer vector

    if nargin < 4
        relPred = 'true';
    end
    valPred_ = sprintf('all(%s > 0)', name);
    if nargin == 5
        valPred_ = [valPred_, ' && ', valPred];
    end
    AddIntVector(name, string, desc, relPred, valPred_);
    
end