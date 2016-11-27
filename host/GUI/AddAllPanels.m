function AddAllPanels()
%% Add all panels to GUI given GUI type,
%  select a default panel

    global guiType GuiTypes
    global startFromScratch plotStyles panIdx
    
    % Add all panels except of the Plot panels
    switch guiType
        case GuiTypes.StartFromScratch
            % START_Arachne -> Start from scratch
            AddCustomParams();
            AddModelParams();
            AddModelAstroParams();
            AddModelTonicParams();
            AddModelStdpParams();
            AddModelXCurParams();
            AddModelModCurParams();
            AddHpcParams();
            AddRngParams();
            AddKineticParams();
            AddInitCondParams();
            AddInitCondAstroParams();
            AddInitCondXCurParams();
            AddMeasuredParams();
            AddMeasuredAstroParams();
            AddMeasuredXCurParams();
        case GuiTypes.ContinueOldSession
            % START_Arachne -> Continue old session
            AddModelAndHpcParamsForContinuation();
            
        case GuiTypes.MonitorBackgroundProcess
            % START_Arachne -> Monitor background process
            AddHpcParamsForMonitoring();
            
        case GuiTypes.TakeOutputData
            % START_Arachne -> Take output data
            AddHpcParamsForTakingOutputData();
            
        case GuiTypes.ScriptTakeSnapshot
            % SCRIPT_TakeSnapshot
            AddHpcParamsForTakingSnapshot();
            
        case GuiTypes.UtilityPlotStdpModels
            % UTILITY_PlotStdpModels
            AddModelParams_for_UTILITY_PlotStdpModels();
            AddModelStdpParams_for_UTILITY_PlotStdpModels();
            
        case GuiTypes.UtilityComputeMaxModelSize
            % UTILITY_ComputeMaxModelSize
            AddCustomParams();
            AddRatioParam_for_UTILITY_ComputeMaxModelSize();
            AddModelParams_for_UTILITY_ComputeMaxModelSize();
            AddModelAstroParams_for_UTILITY_ComputeMaxModelSize();
            AddModelTonicParams_for_UTILITY_ComputeMaxModelSize();
            AddModelStdpParams_for_UTILITY_ComputeMaxModelSize();
            AddHpcParams_for_UTILITY_ComputeMaxModelSize();
            AddMeasuredAstroParams_for_UTILITY_ComputeMaxModelSize();
            
        otherwise
            assert(false, 'Unknown GUI type');
    end
    
    % If necessary, add the Plot panels
    plotSupport = [GuiTypes.StartFromScratch, ...
                   GuiTypes.ContinueOldSession, ...
                   GuiTypes.MonitorBackgroundProcess, ...
                   GuiTypes.ScriptTakeSnapshot, ...
                   GuiTypes.TakeOutputData];
    if any(guiType == plotSupport)
        startFromScratch = (guiType == GuiTypes.StartFromScratch);
        plotStyles = struct('DoNotPlot', 1, 'PlotCurvesSeparately', 2, 'PlotCurvesTogether', 3);
        AddPlotParams();
        AddPlotAstroParams();
        AddPlotXCurParams();
    end
    
    % Select a default panel
    if guiType == GuiTypes.StartFromScratch || guiType == GuiTypes.UtilityComputeMaxModelSize
        panIdx = 2;
    else
        panIdx = 1;
    end
    
end