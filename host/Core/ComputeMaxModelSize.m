function ComputeMaxModelSize(varargin)
%% Common entry point for desktop and mobile versions of Matlab

    % Clear all variables, but do not remove breakpoints
    clearvars -except varargin;
    clearvars -global -except mobileMode;
    
    close all;
    
    clc;
    
    global scalTest backgroundMode
    global pushbutton_OK_Delegate
    global guiType GuiTypes
   
    BasicParams(varargin{:});
    
    backgroundMode = true;
    scalTest = false;
    guiType = GuiTypes.UtilityComputeMaxModelSize;
    pushbutton_OK_Delegate = @SCRIPT_Core;
    
    PrepareAndShowGUI(); 
    
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
