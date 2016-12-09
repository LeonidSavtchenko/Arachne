function AddIntScalar(name, value, desc,   relPred, valPred)
%% Add integer scalar

    if nargin < 4
        % The parameter is always relevant
        relPred = 'true';
    end
    
    % Extend the validation expression to verify that an integer value is provided
    valPred_ = sprintf('rem(%s, 1) == 0', name);
    if nargin == 5
        valPred_ = [valPred_, ' && ', valPred];
    end
    
    unit = 'tg';
    AddRatScalar(name, value, unit, desc, relPred, valPred_);

end
