function g = GenerateScm(suf, g_hat, w_max, numRows, numCols, scmType, sclModel, useSPA, sigma)
%% Generate Synaptic Conductance Matrix.

    global ScmTypes
    
    switch scmType
        case {ScmTypes.AllZeros, ScmTypes.AllEqual}
            disp(['    g_', suf, ' is not generated explicitly because it has primitive structure.']);
            g = nan;
        case ScmTypes.HstDense
            disp(['    Generating g_', suf, ' ...']);
            g = GenerateDenseScm(suf, numRows, numCols, w_max, g_hat, sclModel, useSPA, sigma);
        case ScmTypes.HstSparse
            fprintf('    Generating g_%s ...\n', suf);
            g = GenerateSparseScm(suf, numRows, numCols, w_max, g_hat, sigma);
        otherwise
            disp(['    g_', suf, ' will be generated in HPC kernel.']);
            g = nan;
    end
end