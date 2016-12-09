function UTILITY_PlotStdpModels(varargin)
%% Plot the factors used in STDP mechanisms for four matrices of synaptic conductance,
%  i.e. delta_g_ij / g_ij vs delta_t.
%  Statistics Toolbox is required to adjust range of the figure along y axis.

    AddPaths();

    global mobileMode
    mobileMode = false;
    
    PlotStdpModels(varargin{:});
    
end