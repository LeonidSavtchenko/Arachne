function START_Arachne(varargin)
%% HPC solver of Hodgkin-Huxley-like equations for the system of two coupled ring networks.
%  (This entry point is for desktop version of Matlab.)

    AddPaths();

    global mobileMode
    mobileMode = false;
    
    Arachne(varargin{:});
    
end