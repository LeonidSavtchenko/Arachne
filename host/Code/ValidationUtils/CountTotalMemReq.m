function reqMemPerNode = CountTotalMemReq()
%% This function counts memory required on one node of the cluster (in MB).
    
    global np useSPA num_e num_i enableSTDP freqWinWidth dt scalTest minNP distMatPVH
    global scmType_ee scmType_ie scmType_ei scmType_ii
    global w_ee_max w_ie_max w_ei_max w_ii_max
    global sigma_ee sigma_ie sigma_ei sigma_ii 
    global max_delay_e max_delay_i
    global m_steps setIter max_num_spikes_e_factor max_num_spikes_i_factor
    global gatherCaColormap caColormapPeriodIter
    
    % Remark: We cast int32 parameters to double type to avoid round-off errors
    
    assert(~isempty(scalTest));
    if ~scalTest
        np_worst = double(np);
    else
        np_worst = double(minNP);
    end

    assert(~isempty(useSPA));
    if useSPA
        elemSize = 4;
    else
        elemSize = 8;
    end
    
    reqMemPerNode = 0;
    
    % SCM matrices
    reqMemPerNode = reqMemPerNode + CountMatrixMemReq(double(num_e), double(num_e), np_worst, elemSize, scmType_ee, sigma_ee, w_ee_max, 'ee');
    reqMemPerNode = reqMemPerNode + CountMatrixMemReq(double(num_i), double(num_e), np_worst, elemSize, scmType_ie, sigma_ie, w_ie_max, 'ie');
    reqMemPerNode = reqMemPerNode + CountMatrixMemReq(double(num_e), double(num_i), np_worst, elemSize, scmType_ei, sigma_ei, w_ei_max, 'ei');
    reqMemPerNode = reqMemPerNode + CountMatrixMemReq(double(num_i), double(num_i), np_worst, elemSize, scmType_ii, sigma_ii, w_ii_max, 'ii');
    
    % PVH matrices
    reqMemPerNode_PVH_e = 2 * double(num_e) * double(max_delay_e) * elemSize / 2 ^ 20;
    reqMemPerNode_PVH_i = 2 * double(num_i) * double(max_delay_i) * elemSize / 2 ^ 20;
    assert(~isempty(distMatPVH));
    if distMatPVH
        reqMemPerNode_PVH_e = reqMemPerNode_PVH_e / np_worst;
        reqMemPerNode_PVH_i = reqMemPerNode_PVH_i / np_worst;
    end
    reqMemPerNode = reqMemPerNode + reqMemPerNode_PVH_e + reqMemPerNode_PVH_i;
    
    % Spikes data
    assert(~isempty(setIter));
    if setIter
        max_num_spikes_e = floor(double(num_e) * m_steps * max_num_spikes_e_factor);
        max_num_spikes_i = floor(double(num_i) * m_steps * max_num_spikes_i_factor);
        intSize = 4;
        reqMemPerNode = reqMemPerNode + max_num_spikes_e * (intSize + elemSize) / 2 ^ 20;
        reqMemPerNode = reqMemPerNode + max_num_spikes_i * (intSize + elemSize) / 2 ^ 20;
    end
    
    % Spikes history
    assert(~isempty(enableSTDP));
    if enableSTDP
        freqWinWidth_iter = round(freqWinWidth / dt);
        reqMemPerNode = reqMemPerNode + double(num_e) * freqWinWidth_iter / np_worst / 2 ^ 20;
        reqMemPerNode = reqMemPerNode + double(num_i) * freqWinWidth_iter / np_worst / 2 ^ 20;
    end
    
    % Ca colormap
    assert(~isempty(gatherCaColormap));
    if gatherCaColormap
        numRows = double(num_e);    % = num_a
        numCols = floor(m_steps / double(caColormapPeriodIter));
        reqMemPerNode = reqMemPerNode + numRows * numCols * elemSize / 2 ^ 20;
    end
    
end