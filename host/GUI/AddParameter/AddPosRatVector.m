function AddPosRatVector(name, value, unit, desc,   relPred, valPred)
%% Add positive rational vector

    if nargin < 5
        relPred = 'true';
    end
    valPred_ = sprintf('all(%s > 0)', name);
    if nargin == 6
        valPred_ = [valPred_, ' && ', valPred];
    end
    AddRatVector(name, value, unit, desc, relPred, valPred_);
    
end