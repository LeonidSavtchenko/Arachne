function EvaluateAllRemainders(guiType)
%% Evaluate all remainders given GUI type:
%  check if input parameters are not conflicting,
%  do preprocessing of the parameters.
            
    global outFileName remoteHPC hostParams loadedNodes GuiTypes

    switch guiType
        case GuiTypes.StartFromScratch
            RelParamsDispatcher();
            HpcParamsRemainder();
            ModelParamsRemainder();
            MeasuredParamsRemainder();
            RngParamsRemainder();
            ValidateAndPreprocessParams();
            ParamsPostprocessing();
            CastParams();
        case GuiTypes.ContinueOldSession
            if remoteHPC
                hostParams = strjoin(loadedNodes, ',');
            end
        case {GuiTypes.MonitorBackgroundProcess, GuiTypes.TakeOutputData, GuiTypes.ScriptTakeSnapshot}
            % (empty)
        case {GuiTypes.UtilityPlotStdpModels, GuiTypes.UtilityComputeMaxModelSize}
            ModelParamsRemainder();
        otherwise
            assert(false, 'Unknown GUI type');
    end
    
    if guiType ~= GuiTypes.StartFromScratch
        if ~endsWith(outFileName, '.mat')
            outFileName = [outFileName, '.mat'];
        end
    end
end

function RelParamsDispatcher()

    global sclModel SclModels
    global w_ee_max w_ei_max w_ie_max w_ii_max
    global w_ee_max_bss w_ei_max_bss w_ie_max_bss w_ii_max_bss
    global w_ee_max_bsd w_ei_max_bsd w_ie_max_bsd w_ii_max_bsd
    global scalTest np nt maxNP maxNT
    global hostParams loadedNodes
        
    if sclModel == SclModels.BSS
        w_ee_max = w_ee_max_bss;
        w_ei_max = w_ei_max_bss;
        w_ie_max = w_ie_max_bss;
        w_ii_max = w_ii_max_bss;
    else
        w_ee_max = w_ee_max_bsd;
        w_ei_max = w_ei_max_bsd;
        w_ie_max = w_ie_max_bsd;
        w_ii_max = w_ii_max_bsd;
    end
    
    if scalTest
        np = maxNP;
        nt = maxNT;
    end
    
    hostParams = strjoin(loadedNodes, ',');
end

function ParamsPostprocessing()

    global scmSeeds uSeeds releaseSeeds
    
    scmSeeds = scmSeeds';
    uSeeds = uSeeds';
    releaseSeeds = releaseSeeds';
    
end
