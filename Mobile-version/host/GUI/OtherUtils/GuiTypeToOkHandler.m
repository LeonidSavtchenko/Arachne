function [okHandler, stopAfter] = GuiTypeToOkHandler(guiType)

    global pushbutton_OK_Delegate GuiTypes mobileMode
    
    switch guiType
        case {GuiTypes.StartFromScratch, GuiTypes.ContinueOldSession}
            okHandler = @RunAndMonitor;
            stopAfter = mobileMode;
        case GuiTypes.MonitorBackgroundProcess
            okHandler = @MonitorBackgroundProcess;
            stopAfter = mobileMode;
        case GuiTypes.TakeOutputData
            okHandler = @()GrabReadAndVisualizeResults(false);
            stopAfter = true;
        case {GuiTypes.ScriptTakeSnapshot, GuiTypes.UtilityPlotStdpModels, GuiTypes.UtilityComputeMaxModelSize}
            okHandler = pushbutton_OK_Delegate;
            stopAfter = true;
        otherwise
            assert(false, 'Unknown GUI type');
    end
    
end