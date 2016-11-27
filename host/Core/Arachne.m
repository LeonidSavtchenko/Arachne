function Arachne(varargin)
%% Common entry point for desktop and mobile versions of Matlab

    % Clear all variables, but do not remove breakpoints
    clearvars -except varargin;
    clearvars -global -except mobileMode;
    
    close all;
    
    clc;
    
    global scalTest
    global continuationMode guiType GuiTypes
    global Scenarios
    
    scalTest = false;
    
    BasicParams(varargin{:});
    
    %% Begin with scenario selection (start from scratch, continue from the same point and so on)
    scenario = SelectScenario();
    
    %% Choose a GUI to show
    switch scenario
        case Scenarios.TakeAndVisualizeResults
            guiType = GuiTypes.TakeOutputData;
        case Scenarios.PrepareParamsRunAndMonitor
            if ~continuationMode
                guiType = GuiTypes.StartFromScratch;
            else
                guiType = GuiTypes.ContinueOldSession;
            end
        case Scenarios.MonitorBackgroundProcess;
            guiType = GuiTypes.MonitorBackgroundProcess;
        case Scenarios.DoNothing
            return
    end
    
    %% Show the GUI
    PrepareParamsWithGUI();
    
end