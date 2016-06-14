function START_GammaSimulator()
%% Solve the system of Hodgkin-Huxley-like equations using the midpoint method

    % Clear all variables, but do not remove breakpoints
    clearvars;
    clearvars -global;
    
    close all;
    
    clc;
    
    global scalTest
    global continuationMode guiType GuiTypes
    global Scenarios
    
    scalTest = false;
    
    AddPaths();

    BasicParams();
    
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