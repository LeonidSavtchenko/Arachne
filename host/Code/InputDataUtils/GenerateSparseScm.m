function out = GenerateSparseScm(suf, numRows, numCols, w_max, g_hat, sigma)
%% Generate synaptic conductance matrix with sparse structure.
%  Input parameters:
%  numRows - number of rows in the matrix,
%  numCols - number of columns in the matrix,
%  w_max, g_hat - parameters used for matrix elements generation.
    
    nw = max(numRows, numCols);
    if rem(nw, 2) == 0
        nw = nw + 1;
    end;
    w = WeightVector(nw, w_max, sigma, suf);
    c = (nw + 1) / 2;
    
    i_ = [];
    j_ = [];
    s_ = [];
    idx = 1;
    for i = 1 : numRows
        for j = 1 : numCols
            index = c + div_round(((j - 1) * (numRows - 1) - (i - 1) * (numCols - 1)) * (nw - 1), (numRows - 1) * (numCols - 1));
            while index < 1
                index = index + nw;
            end
            while index > nw
                index = index - nw;
            end
            if (rand() <= w(index))
               i_(idx) = i; %#ok<AGROW>
               j_(idx) = j; %#ok<AGROW>
               s_(idx) = g_hat; %#ok<AGROW>
               idx = idx + 1;
            end
        end
    end
    out = sparse(i_, j_, s_, numRows, numCols);
    
end