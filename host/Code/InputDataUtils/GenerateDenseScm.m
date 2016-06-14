function g = GenerateDenseScm(suf, numRows, numCols, w_max, g_hat, sclModel, useSPA, sigma)

    global SclModels
    
%% Generate Dense Synaptic Conductance Matrix
    switch sclModel
        case SclModels.BSS
            % Bell-shaped strength and uniform density of connections
            % (elements of the matrices will be multiplied by weight factors below)
            g = ones(numRows, numCols);
        case SclModels.BSD
            % Uniform strength and bell-shaped density of connections
            % (some elements of the matrices will be replaced with zeros below)
            g = g_hat * ones(numRows, numCols);
    end
    
    nw = max(numRows, numCols);
    if rem(nw, 2) == 0
        nw = nw + 1;
    end;
    w = WeightVector(nw, w_max, sigma, suf);
    c = (nw + 1) / 2;
    
    for i = 1 : numRows
        for j = 1 : numCols
            index = c + div_round(((j - 1) * (numRows - 1) - (i - 1) * (numCols - 1)) * (nw - 1), (numRows - 1) * (numCols - 1));
            while index < 1
                index = index + nw;
            end
            while index > nw
                index = index - nw;
            end
            switch sclModel
                case SclModels.BSS
                    % Bell-shaped strength and uniform density of connections
                    g(i, j) = g(i, j) * w(index);
                case SclModels.BSD
                    % Uniform strength and bell-shaped density of connections
                    if rand() > w(index)
                        g(i, j) = 0;
                    end
            end
        end
    end
    if useSPA
        g = single(g);
    end;
end

