function ValidateAndPreprocessParams()
%% Check if input parameters are not conflicting.
%  Do preprocessing of the parameters, e.g.
%  1) if some bit matrices are used, then align num_e and num_i properly;
%  2) if some cells are watched, then switch from 1-based indexes used in Matlab to 0-based indexes used in C++;
%  and so on.

    global useGUI
    
    % Model parameters
    global g_hat_ee g_hat_ei g_hat_ie g_hat_ii num_e num_i
    global w_ee_max w_ii_max w_ei_max w_ie_max
    global releaseProb_ei releaseProb_ie
    
    % HPC parameters
    global ScmTypes scmType_ee scmType_ei scmType_ie scmType_ii
    global useSPA saveInput2Output outFileName
    
    % RNG parameters
    global use32BitRng
    
    % Measured parameters
    global watchedCellIdx_e watchedCellIdx_i watchedCellNum_e watchedCellNum_i
    global watchedSynIdx_ee watchedSynIdx_ei watchedSynIdx_ie watchedSynIdx_ii
    global watchedSynNum_ee watchedSynNum_ei watchedSynNum_ie watchedSynNum_ii
    global enableAstro watchedAstroIdx watchedAstroNum 
    global enableExtraCurrent_e watchedExtraCurrentIdx_e watchedExtraCurrentNum_e 
    global enableExtraCurrent_i watchedExtraCurrentIdx_i watchedExtraCurrentNum_i 
      
    CheckScmParams('ee', g_hat_ee, w_ee_max, scmType_ee);
    CheckScmParams('ei', g_hat_ei, w_ei_max, scmType_ei);
    CheckScmParams('ie', g_hat_ie, w_ie_max, scmType_ie);
    CheckScmParams('ii', g_hat_ii, w_ii_max, scmType_ii);
        
    %% Given Synaptic Conductance Matrix Types,
    %  check if alignment of number of neurons of a type is required.
    %  If so, do the alignment.
    
    % Compute block size for num_i
    if scmType_ie == ScmTypes.KrnOneBit || scmType_ii == ScmTypes.KrnOneBit
        blockSize_i = 64;
    else
        blockSize_i = 1;
    end
    
    % Compute block size for num_e
    if scmType_ee == ScmTypes.KrnOneBit || scmType_ei == ScmTypes.KrnOneBit
        blockSize_e = 64;
    else
        blockSize_e = 1;
    end

    % Do the alignment
    num_i_aligned = num_i;
    num_e_aligned = num_e;
    if blockSize_e ~= 1 || blockSize_i ~= 1
        if rem(num_i, blockSize_i) ~= 0
            num_i_aligned = floor(double(num_i) / blockSize_i) * blockSize_i;
        end
        if rem(num_e, blockSize_e) ~= 0
            num_e_aligned = floor(double(num_e) / blockSize_e) * blockSize_e;
        end
        
        if num_i_aligned ~= num_i || num_e_aligned ~= num_e
            pattern = [...
                'The scmTypes specified require num_e to be evenly divisible by %i and num_i to be evenly divisible by %i.\n', ...
                'The numbers of neurons were aligned to fit the requirements as follows:\n', ...
                'num_e = %i (was: %i)\n', ...
                'num_i = %i (was: %i)\n'];
            msg = sprintf(pattern, blockSize_e, blockSize_i, num_e_aligned, num_e, num_i_aligned, num_i);
            disp(msg);
            releaseProb_ei(num_e_aligned + 1 : num_e) = [];
            releaseProb_ie(num_i_aligned + 1 : num_i) = [];
            num_e = num_e_aligned;
            num_i = num_i_aligned;
        end
   
        if num_i_aligned == 0 || num_e_aligned == 0
            error('Cannot do simulation with zero number of neurons of a type.');
        end
    end
    
    %% Preprocess and validate the parameters specifying watched cells and synapses
    watchedCellNum_e = length(watchedCellIdx_e);
    watchedCellNum_i = length(watchedCellIdx_i);
    watchedAstroNum = length(watchedAstroIdx);
    watchedExtraCurrentNum_e = length(watchedExtraCurrentIdx_e);
    watchedExtraCurrentNum_i = length(watchedExtraCurrentIdx_i);
    watchedSynNum_ee = size(watchedSynIdx_ee, 1);
    watchedSynNum_ei = size(watchedSynIdx_ei, 1);
    watchedSynNum_ie = size(watchedSynIdx_ie, 1);
    watchedSynNum_ii = size(watchedSynIdx_ii, 1);
    watchedCellIdx_e = CheckAndPreprocWathedCellParams(watchedCellIdx_e, num_e, 'e-cell');
    watchedCellIdx_i = CheckAndPreprocWathedCellParams(watchedCellIdx_i, num_i, 'i-cell');
    if enableAstro
        watchedAstroIdx = CheckAndPreprocWathedCellParams(watchedAstroIdx, num_e, 'astro cell');
    end
    watchedSynIdx_ee = CheckAndPreprocWathedSynParams(watchedSynIdx_ee, num_e, num_e, 'ee');
    watchedSynIdx_ei = CheckAndPreprocWathedSynParams(watchedSynIdx_ei, num_e, num_i, 'ei');
    watchedSynIdx_ie = CheckAndPreprocWathedSynParams(watchedSynIdx_ie, num_i, num_e, 'ie');
    watchedSynIdx_ii = CheckAndPreprocWathedSynParams(watchedSynIdx_ii, num_i, num_i, 'ii');
    if enableExtraCurrent_e
        watchedExtraCurrentIdx_e = CheckAndPreprocWathedCellParams(watchedExtraCurrentIdx_e, num_e, 'e-cell');
    end
    if enableExtraCurrent_i
        watchedExtraCurrentIdx_i = CheckAndPreprocWathedCellParams(watchedExtraCurrentIdx_i, num_i, 'i-cell');
    end
    
    %% Validate parameters of random number generators
    assert(~(useSPA && ~use32BitRng), 'Cannot use fine-grained random number generator std::mt19937_64 with single precision arithmetics.');
    
    %% Preprocess output MAT-file name
    if ~endsWith(outFileName, '.mat')
        outFileName = [outFileName, '.mat'];
    end
    
    %% Preprocess parameters depending on useGUI
    if ~useGUI
       saveInput2Output = false; 
    end
end

function CheckScmParams(suf, g_hat, w_max, scmType)
%% Check Synaptic Conductance Matrix parameters

    global useGUI ScmTypes enableSTDP SclModels sclModel gatherSCM
    
    assert(HasField(ScmTypes, scmType), ...
        ['Unsupported scmType == ', num2str(scmType), '.']);
    
    assert(~(sclModel == SclModels.BSD && scmType == ScmTypes.AllZeros && g_hat ~= 0), ...
        ['Improper parameter combination detected: scmType_', suf, ' == ScmTypes.AllZeros && g_hat_', suf, ' ~= 0.']);
    
    assert(~(scmType == ScmTypes.AllZeros && w_max ~= 0), ...
        ['Improper parameter combination detected: scmType_', suf, ' == ScmTypes.AllZeros && w_max_', suf, ' ~= 0.']);
    
    assert(~(scmType ~= ScmTypes.AllZeros && w_max == 0), ...
        ['Not optimal parameter combination detected: scmType_', suf, ' ~= ScmTypes.AllZeros && w_max_', suf, ' == 0. Please specify scmType_', suf, ' == ScmTypes.AllZeros for performance and memory usage.']);
    
    assert(~(sclModel == SclModels.BSD && scmType ~= ScmTypes.AllZeros && g_hat == 0), ...
        ['Not optimal parameter combination detected: scmType_', suf, ' ~= ScmTypes.AllZeros && g_hat_', suf, ' == 0. Please specify scmType_', suf, ' == ScmTypes.AllZeros for performance and memory usage.']);
    
    if ~useGUI
        assert(~(~enableSTDP && gatherSCM), ...
            'Option gatherSCM == true is valid only for enableSTDP == true.')
    elseif ~enableSTDP
        gatherSCM = false;
    end
    
    assert(~(enableSTDP && scmType ~= ScmTypes.HstDense && scmType ~= ScmTypes.KrnDense), ...
        ['Hebbian correction is not supported for scmType == ScmTypes.', GetFieldName(ScmTypes, scmType), '.']);
    
    assert(~(sclModel == SclModels.BSS && (scmType == ScmTypes.HstSparse || scmType == ScmTypes.KrnSparse || scmType == ScmTypes.KrnOneBit)), ...
        ['Bell-shaped strength of connections is not supported for scmType == ScmTypes.', GetFieldName(ScmTypes, scmType), '.']);
end

function watchedCellIdx = CheckAndPreprocWathedCellParams(watchedCellIdx, num, name)
%% Check and preprocess parameters of wathed cells

    if ~isempty(watchedCellIdx)
        
        pat1 = 'Some index of watched %s is out of range [1, %i].';
        pat2 = 'The index %i appears more than once in the array of indexes of watched %ss.';
    
        watchedCellIdx = sort(watchedCellIdx);
        
        assert(watchedCellIdx(1) >= 1 && watchedCellIdx(end) <= num, sprintf(pat1, name, num));
        
        for i = 1 : length(watchedCellIdx) - 1
            assert(watchedCellIdx(i) ~= watchedCellIdx(i + 1), sprintf(pat2, watchedCellIdx(i), name));
        end
        
        % Vector-columns are used in C++
        watchedCellIdx = watchedCellIdx';
    end
    
end

function watchedSynIdx = CheckAndPreprocWathedSynParams(watchedSynIdx, numRows, numCols, name)
%% Check and preprocess parameters of watched synapses

    if ~isempty(watchedSynIdx)
        
        pat1 = 'Some %s index for watched %s-synapses is out of range [1, %i].';
        pat2 = 'The %s-synapse with position {row = %i, column = %i} is specified more than once in the array of watched synapses.';
    
        % Firstly sort it by 2nd dimension (the watched synapse column),
        % then sort it by 1nd dimension (the watched synapse row)
        watchedSynIdx = sortrows(watchedSynIdx);
        
        % Make sure that all row indexes are in range
        assert(watchedSynIdx(1, 1) >= 1 && watchedSynIdx(end, 1) <= numRows, sprintf(pat1, 'row', name, numRows));
        
        for i = 1 : size(watchedSynIdx, 1)
            % Make sure that this column index is in range
            assert(watchedSynIdx(i, 2) >= 1 && watchedSynIdx(i, 2) <= numCols, sprintf(pat1, 'column', name, numCols));

            % Make sure that there are no duplicates
            if i ~= size(watchedSynIdx, 1)
                assert(any(watchedSynIdx(i, :) ~= watchedSynIdx(i + 1, :)), sprintf(pat2, name, watchedSynIdx(i, 1), watchedSynIdx(i, 2)));
            end
        end
    end
    
end