function UTILITY_MOBILE_PlotStdpModels(varargin)
%% Plot the factors used in STDP mechanisms for four matrices of synaptic conductance,
%  i.e. delta_g_ij / g_ij vs delta_t.
%  Statistics Toolbox is required to adjust range of the figure along y axis.

    AddPaths();

    % Any GUI, "input", "questdlg" or "uigetfile" functions calls will be avoided,
    % because Matlab mobile does not support interaction with user at runtime.
    global mobileMode
    mobileMode = true;
    
    PlotStdpModels(varargin{:});
    
end