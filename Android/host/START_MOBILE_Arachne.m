function START_MOBILE_Arachne(varargin)
%% HPC solver of Hodgkin-Huxley-like equations for the system of two coupled ring networks.
%  (This entry point is for mobile version of Matlab.)

    AddPaths();

    % Any GUI, "input", "questdlg" or "uigetfile" functions calls will be avoided,
    % because Matlab mobile does not support interaction with user at runtime.
    global mobileMode
    mobileMode = true;
    
    Arachne(varargin{:});
    
end