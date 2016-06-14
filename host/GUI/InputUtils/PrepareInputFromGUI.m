function PrepareInputFromGUI()

    global params
    global continuationMode ScmTypes SclModels %#ok<NUSED>
    global m_steps delta_ee delta_ei delta_ie delta_ii max_delay_e max_delay_i %#ok<NUSED>
    global w_ee_max w_ii_max w_ei_max w_ie_max sigma_ee sigma_ei sigma_ie sigma_ii 
    global watchedCellNum_e watchedCellNum_i opNum watchedAstroNum %#ok<NUSED>
    global watchedSynNum_ee watchedSynNum_ei watchedSynNum_ie watchedSynNum_ii %#ok<NUSED>
    global stdpParamsNum stdpCommonParamsNum %#ok<NUSED>
    global stdp_factor factorNum %#ok<NUSED>
    global size_factor t_start_factor t_end_factor %#ok<NUSED>
    global distMatPVH
    
    global watchedExtraCurrentNum_e watchedExtraCurrentNum_i 
    
    % Referred from relevancy prediates
    global startFromScratch %#ok<NUSED>
    
    %% Check memory requirements taking into scope that
    %  the matrices are the biggest objects used in simulation.
    %  Do not start simulation if there is no enough physical memory on cluster nodes.
    CheckMemReq();
    
    %% Add all parameters to current workspace
    % Loop by panels
    for panIdx_ = 1 : length(params)
        % Loop by all parameters of this panel
        for parIdx = 1 : length(params{panIdx_})
            varname = params{panIdx_}{parIdx}.name;
            eval(['global ', varname]);        
        end
    end
    
    %% Specify synaptic conductances
    set1 = [scmType_ee, scmType_ie, scmType_ei, scmType_ii];
    set2 = [ScmTypes.AllZeros, ScmTypes.AllEqual, ScmTypes.HstDense, ScmTypes.HstSparse];
    if ~isempty(intersect(set1, set2))
        % Generate synaptic conductance matrices
        disp('Generating synaptic conductance matrices ...');
        
        assert(rem(num_e, 2) == 0, 'rem(num_e, 2) == 0');
        assert(rem(num_i, 2) == 0, 'rem(num_i, 2) == 0');
        
        g_ee = GenerateScm('ee', g_hat_ee, w_ee_max, num_e, num_e, scmType_ee, sclModel, useSPA, sigma_ee); %#ok<NASGU>
        g_ei = GenerateScm('ei', g_hat_ei, w_ei_max, num_e, num_i, scmType_ei, sclModel, useSPA, sigma_ei); %#ok<NASGU>
        g_ie = GenerateScm('ie', g_hat_ie, w_ie_max, num_i, num_e, scmType_ie, sclModel, useSPA, sigma_ie); %#ok<NASGU>
        g_ii = GenerateScm('ii', g_hat_ii, w_ii_max, num_i, num_i, scmType_ii, sclModel, useSPA, sigma_ii); %#ok<NASGU>
    end
    
    %% Maximum values of matrix elements
    switch sclModel
        case SclModels.BSS
            g_max_ee = 2 * w_ee_max; %#ok<NASGU>
            g_max_ii = 2 * w_ii_max; %#ok<NASGU>
            g_max_ei = 2 * w_ei_max; %#ok<NASGU>
            g_max_ie = 2 * w_ie_max; %#ok<NASGU>
        case SclModels.BSD
            g_max_ee = 2 * g_hat_ee; %#ok<NASGU>
            g_max_ii = 2 * g_hat_ii; %#ok<NASGU>
            g_max_ei = 2 * g_hat_ei; %#ok<NASGU>
            g_max_ie = 2 * g_hat_ie; %#ok<NASGU>
    end
    
    %% Save all necessary input data to file "input.mat"
    
    input = {};
    
    % Prepare list of variables from GUI
    
    % Loop by panels
    for panIdx_ = 1 : length(params)
        % Loop by all parameters of this panel
        for parIdx = 1 : length(params{panIdx_})
            relPred = params{panIdx_}{parIdx}.relPred;
            if ischar(relPred)
                relPred = eval(relPred);
            end
            if relPred
                varname = params{panIdx_}{parIdx}.name;
                input = [input; varname]; %#ok<AGROW>
            end
        end
    end
    
    % Add non-optional variables declared outside GUI
    input = [input; ...
        % HPC parameters
        'continuationMode'; 'backgroundMode'; 'distMatPVH'; ...
        % Model parameters
        'g_max_ee'; 'g_max_ei'; 'g_max_ie'; 'g_max_ii'; ...
        'delta_ee'; 'delta_ii'; 'delta_ei'; 'delta_ie'; 'max_delay_e'; 'max_delay_i'; ...
        'w_ee_max'; 'w_ii_max'; 'w_ei_max'; 'w_ie_max'; ...
        'sigma_ee'; 'sigma_ei'; 'sigma_ie'; 'sigma_ii'; ...    
        % Measured parameters
        'watchedCellNum_e'; 'watchedCellNum_i'; ...
        'watchedSynNum_ee'; 'watchedSynNum_ei'; 'watchedSynNum_ie'; 'watchedSynNum_ii'; ...
        'opNum';
    ];
    
    % Add all optional variables to the list
    
    % Synaptic conductance matrices
    if scmType_ee == ScmTypes.HstDense || scmType_ee == ScmTypes.HstSparse
        input = [input; 'g_ee'];
    end
    if scmType_ie == ScmTypes.HstDense || scmType_ie == ScmTypes.HstSparse
        input = [input; 'g_ie'];
    end
    if scmType_ei == ScmTypes.HstDense || scmType_ei == ScmTypes.HstSparse
        input = [input; 'g_ei'];
    end
    if scmType_ii == ScmTypes.HstDense || scmType_ii == ScmTypes.HstSparse
        input = [input; 'g_ii'];
    end
    
    % Seeds
    set1 = [scmType_ee, scmType_ie, scmType_ei, scmType_ii];
    set2 = [ScmTypes.KrnDense, ScmTypes.KrnSparse, ScmTypes.KrnOneBit, ScmTypes.KrnInPlace];
    if ~isempty(intersect(set1, set2))
        input = [input; 'scmSeeds'];
    end
    
    % Other optional variables
    if enableSTDP
        if ~importSTDP
            input = [input; 'stdpParamsNum'];
        else
            input = [input; 'stdp_factor'; 'factorNum'; 'size_factor'; 't_start_factor'; 't_end_factor'];
        end
        input = [input; 'stdpCommonParamsNum'];
    end
    if enableAstro
        input = [input; 'watchedAstroNum'];
    end
   
    if enableExtraCurrent_e
        input = [input; 'watchedExtraCurrentNum_e'];
    end
    
    if enableExtraCurrent_i
        input = [input; 'watchedExtraCurrentNum_i'];
    end
    
    if fakeMPI
        input = [input; 'distMatPVH'];
    end
    
    if distMatPVH
        input = [input; 'saveIntermMat'];
    end
    
    % Save the file
    disp('Creating input MAT-file ...');
    save('input.mat', input{:});

end