function numElem = EstimateSparseMatrixNNZ(m, n, sigma, w_max, suf)
%% Compute estimate for the number of nonzero elements in a sparse matrix of size m x n
%  populated based on the bell-shaped density model given sigma, w_max and suf.
%  The number becomes overestimated in the case when w_max > 1.
%
%  See derivation of the integral in the following file:
%  gs\tests\Derivation_EstimateSparseMatrixNNZ.nb

    nw = max(m, n);
    if rem(nw, 2) == 0
        nw = nw + 1;
    end
    
    A = sqrt(0.5) * (nw - 1) / sigma;
    B = A ^ 2 / (m - 1) / (n - 1);
    
    integral = (exp(-A ^ 2) - 1 + A * sqrt(pi) * erf(A)) / B;
    
    numElem = round(w_max * integral);
    
    if strcmp(suf, 'ee') || strcmp(suf, 'ii')
        % Take into scope that all diagonal elements are zeros
        % (if we are here, then m == n)
        numElem = numElem - m;
        if numElem < 0
            numElem = 0;
        end
    end
    
end