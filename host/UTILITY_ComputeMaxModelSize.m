function UTILITY_ComputeMaxModelSize()
%% Compute the maximum model size (i.e. num_e and num_i) given physical memory limit and ratio num_e / num_i

    % Clear all variables, but do not remove breakpoints
    clearvars;
    clearvars -global;
    
    close all;
    
    clc;
    
    global scalTest backgroundMode
    global pushbutton_OK_Delegate
    global guiType GuiTypes
   
    AddPaths();
    
    BasicParams();
    
    backgroundMode = true;
    scalTest = false;
    guiType = GuiTypes.UtilityComputeMaxModelSize;
    pushbutton_OK_Delegate = @SCRIPT_Core;
    
    PrepareParamsWithGUI(); 
    
end

function SCRIPT_Core()

    global i2eRatio
    global setIter
    global num_e num_i
        
    MeasuredParamsRemainder();
    ModelParamsRemainder();
    HpcParamsRemainder();

    setIter = true;
    
    MaxModelSize(i2eRatio);

    reqMemPerNode = CountTotalMemReq();
    assert(~isempty(reqMemPerNode));
    str = sprintf('The maximum number of e-neurons is %i.\nThe maximum number of i-neurons is %i.\nThe memory required per cluster node is %i MB.', num_e, num_i, round(reqMemPerNode));
    
    disp(str);
    
end
