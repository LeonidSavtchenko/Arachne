function w = WeightVector(nw, w_max, sigma, suf)
%Generates weight vector
    c = (nw + 1) / 2;
    i = 1 : nw;
    w = normpdf(i - 1, (nw - 1) / 2, sigma);
    cutPoint = div_round(nw, 4);
    w(c + cutPoint + 1 : end) = 0;
    w(1 : c - cutPoint - 1) = 0;
    if strcmp(suf, 'ee') || strcmp(suf, 'ii')
        w(c) = 0;
        w = w_max * w / w(c + 1);
    else
        w = w_max * w / w(c);
    end
end

