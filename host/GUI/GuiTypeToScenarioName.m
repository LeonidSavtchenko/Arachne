function scenario = GuiTypeToScenarioName(guiType)

    global GuiTypes
    
    switch guiType
        case GuiTypes.TakeOutputData
            scenario = 'take';
        case GuiTypes.StartFromScratch
            scenario = 'start';
        case GuiTypes.ContinueOldSession
            scenario = 'continue';
        case GuiTypes.MonitorBackgroundProcess
            scenario = 'monitor';
        case GuiTypes.ScriptTakeSnapshot
            scenario = 'snapshot';
        case {GuiTypes.UtilityPlotStdpModels, GuiTypes.UtilityComputeMaxModelSize}
            scenario = 'utility';
        otherwise
            assert(false, 'Unknown GUI type');
    end
    
end