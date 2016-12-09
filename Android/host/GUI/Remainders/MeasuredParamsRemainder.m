function MeasuredParamsRemainder()

    global opRadii opAngles opNum
    global num_e num_i radius_e radius_i useSPA

    %% Check parameters
    ulp = 100;
    if useSPA
        precision_e = ulp * eps(single(radius_e));
        precision_i = ulp * eps(single(radius_i));
    else
        precision_e = ulp * eps(radius_e);
        precision_i = ulp * eps(radius_i);
    end
    opNum = length(opRadii);
    msgPattern = 'The observation point with radius %i and angle %i is too close by one of %s-neurons.';
    for i = 1 : opNum
        p1 = radius_e ^ 2 + opRadii(i) ^ 2;
        p2 = 2 * radius_e * opRadii(i);
        for j = 1 : num_e
            assert(p1 - p2 * cos(opAngles(i) - 2 * pi * (j - 1) / num_e) > precision_e, sprintf(msgPattern, opRadii(i), opAngles(i), 'e'));
        end
        p1 = radius_i ^ 2 + opRadii(i) ^ 2;
        p2 = 2 * radius_i * opRadii(i);
        for j = 1 : num_i
            assert(p1 - p2 * cos(opAngles(i) - 2 * pi * (j - 1) / num_i) > precision_i, sprintf(msgPattern, opRadii(i), opAngles(i), 'i'));
        end
    end

    opRadii = opRadii';
    opAngles = opAngles';
    
end