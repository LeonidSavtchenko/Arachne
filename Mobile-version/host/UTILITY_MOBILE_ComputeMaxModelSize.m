function UTILITY_MOBILE_ComputeMaxModelSize(varargin)
%% Compute the maximum model size (i.e. num_e and num_i) given physical memory limit and ratio num_e / num_i

    AddPaths();

    % Any GUI, "input", "questdlg" or "uigetfile" functions calls will be avoided,
    % because Matlab mobile does not support interaction with user at runtime.
    global mobileMode
    mobileMode = true;
    
    ComputeMaxModelSize(varargin{:});
    
end