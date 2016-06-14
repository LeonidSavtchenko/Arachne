function valid = valPred_stdpCommonParams(stdpCommonParams)
%% Validation predicate for "stdpCommonParams" matrix

    preFreqFactor   = stdpCommonParams(1, :);
    postFreqFactor  = stdpCommonParams(2, :);
    freqTermDivisor = stdpCommonParams(3, :);
    mainTermDivisor = stdpCommonParams(4, :);
    
    valid = (all(preFreqFactor > 0) && all(postFreqFactor > 0) && all(freqTermDivisor ~= 0) && all(mainTermDivisor ~= 0));

end
