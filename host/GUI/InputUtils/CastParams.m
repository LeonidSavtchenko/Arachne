function CastParams()

    global params useSPA 
    global w_ee_max w_ii_max w_ei_max w_ie_max sigma_ee sigma_ei sigma_ie sigma_ii
    global watchedCellNum_e watchedCellNum_i opNum watchedAstroNum
    global watchedSynNum_ee watchedSynNum_ei watchedSynNum_ie watchedSynNum_ii
    global watchedCellIdx_e watchedCellIdx_i watchedAstroIdx freqDelay
    global max_num_spikes_e_factor max_num_spikes_i_factor
    global stdpParams stdpCommonParams stdpParamsNum stdpCommonParamsNum
    global importSTDP stdp_factor t_start_factor t_end_factor
    global watchedExtraCurrentNum_e watchedExtraCurrentIdx_e
    global watchedExtraCurrentNum_i watchedExtraCurrentIdx_i
     
    %% Add all parameters to current workspace and cast scalars to single type if Single Precision Arithmetics must be used
    % Loop by panels
    for panIdx_ = 1 : length(params)
        % Loop by all parameters of this panel
        for parIdx = 1 : length(params{panIdx_})
            varname = params{panIdx_}{parIdx}.name;
            eval(['global ', varname]);
            handlers = params{panIdx_}{parIdx}.handlers;
            
            try
                style = get(handlers(2), 'Style');
            catch
                style = 'uitable';
            end
            if useSPA && (strcmp(style, 'edit') || strcmp(style, 'uitable'))
                % The check isnumeric is necessary for string vectors and function handles
                eval(sprintf('if isnumeric(%s) %s = single(%s); end', varname, varname, varname));
                % Cast to int32 if needed
                eval(sprintf('%s = params{panIdx_}{parIdx}.caster(%s);', varname, varname));
            elseif strcmp(style, 'checkbox')
                eval(sprintf('%s = logical(%s);', varname, varname));
            elseif strcmp(style, 'popupmenu')
                eval(sprintf('%s = int32(%s);', varname, varname));
            end
        end
    end

    %% Cast to float all scalars not defined by user
    if useSPA
        w_ee_max = single(w_ee_max);
        w_ei_max = single(w_ei_max);
        w_ie_max = single(w_ie_max);
        w_ii_max = single(w_ii_max);
        sigma_ee = single(sigma_ee);
        sigma_ei = single(sigma_ei);
        sigma_ie = single(sigma_ie);
        sigma_ii = single(sigma_ii);
        
        if importSTDP
            stdp_factor = single(stdp_factor);
            t_start_factor = single(t_start_factor);
            t_end_factor = single(t_end_factor);
        end
    end

    %% Cast variables that were not created in GUI
    opNum = int32(opNum);
    stdpParamsNum = int32(size(stdpParams, 1));
    stdpCommonParamsNum = int32(size(stdpCommonParams, 1));
    
    watchedCellNum_e = int32(watchedCellNum_e);
    watchedCellNum_i = int32(watchedCellNum_i);
    watchedCellIdx_e = int32(watchedCellIdx_e);
    watchedCellIdx_i = int32(watchedCellIdx_i);
    
    watchedSynNum_ee = int32(watchedSynNum_ee);
    watchedSynNum_ei = int32(watchedSynNum_ei);
    watchedSynNum_ie = int32(watchedSynNum_ie);
    watchedSynNum_ii = int32(watchedSynNum_ii);
  
    watchedAstroNum = int32(watchedAstroNum);
    watchedAstroIdx = int32(watchedAstroIdx);

    watchedExtraCurrentNum_e = int32(watchedExtraCurrentNum_e);
    watchedExtraCurrentIdx_e = int32(watchedExtraCurrentIdx_e);
   
    watchedExtraCurrentNum_i = int32(watchedExtraCurrentNum_i);
    watchedExtraCurrentIdx_i = int32(watchedExtraCurrentIdx_i);
   
    
    %% Cast the parameter that has different meaning in GUI and worker
    freqDelay = int32(freqDelay);
    
    %% Cast the parameters that always have double type
    max_num_spikes_e_factor = double(max_num_spikes_e_factor);
    max_num_spikes_i_factor = double(max_num_spikes_i_factor);
    
end