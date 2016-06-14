function valid = valPred_stdpParams(stdpParams)
%% Validation predicate for "stdpParams" matrix

    S         = stdpParams(2, :);
    tau2      = stdpParams(5, :);
    S2        = stdpParams(6, :);
    SinPeriod = stdpParams(15, :);
    damper    = stdpParams(16, :);
    maxAbsdW  = stdpParams(17, :);
    
    valid = (all(S == floor(S)) && all(tau2 ~= 0) && all(S2 == floor(S2)) && all(SinPeriod ~= 0) && all(damper > 0) && all(maxAbsdW > 0));
    
end